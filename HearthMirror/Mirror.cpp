//
//  Mirror.cpp
//  Mirror
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef __APPLE__
#define NOMINMAX
#endif

#include <codecvt>

#include "Mirror.hpp"
#include "memhelper.h"
#include "Helpers/offsets.h"

#include "Mono/MonoImage.hpp"
#include "Mono/MonoObject.hpp"
#include "Mono/MonoStruct.hpp"
#include <numeric>
#include <algorithm>
#include <unordered_set>

namespace hearthmirror {

    typedef std::vector<std::string> HMObjectPath;

    struct _mirrorData {
        HANDLE task;
        MonoImage* monoImage = NULL;
    };

    Mirror::Mirror(int pid, bool isBlocking) {
        this->m_mirrorData = new _mirrorData();
		if (initWithPID(pid, isBlocking) != 0) {
			throw std::runtime_error("Failed to init Hearthmirror");
		}
    }

    Mirror::~Mirror() {
        if (this->m_mirrorData) {
            delete m_mirrorData->monoImage;
            delete m_mirrorData;
        }
    }

    int Mirror::initWithPID(int pid, bool isBlocking) {
        if (m_mirrorData->monoImage) delete m_mirrorData->monoImage;
        m_mirrorData->monoImage = NULL;

		// get handle to process
        return MonoImage::getMonoImage(pid, isBlocking, &m_mirrorData->task, &m_mirrorData->monoImage);
    }

    std::vector<RewardData*> parseRewards(MonoValue items);
    MonoValue getCurrentBrawlMission(MonoImage* monoImage);

	static MonoValue nullMonoValue(0);

#pragma mark - Helper functions

    long long intValue(MonoValue& mv) {
        if (mv.type == MONO_TYPE_I1) {
            return mv.value.i8;
        } else if (mv.type == MONO_TYPE_I2) {
            return mv.value.i16;
        } else if (mv.type == MONO_TYPE_I4) {
            return mv.value.i32;
        } else if (mv.type == MONO_TYPE_I8) {
            return mv.value.i64;
        }
        return 0;
    }

    MonoValue getObject(const MonoValue& from, const HMObjectPath& path) {

        if (IsMonoValueEmpty(from) || path.size() < 1) return nullMonoValue;

		MonoValue mv = from; // local copy

        for (unsigned int i = 0; i< path.size(); i++) {
            MonoObject* mo = mv.value.obj.o;
            mv = (*mo)[path[i]];
            if (IsMonoValueEmpty(mv)) {
                if (i>0) delete mo;
                return nullMonoValue;
            }

            if (i>0) delete mo; // retain the original "from" object
        }
        return mv;
    }

    /** Helper function to find MonoObject at the given path. */
    MonoValue getObject(const HMObjectPath& path, MonoImage* monoImage) {
        if (path.size() < 2) return nullMonoValue;

        MonoClass* baseclass = monoImage->get(path[0]); // no need to free
        if (!baseclass) return nullMonoValue;

        MonoValue mv = (*baseclass)[path[1]];
        if (IsMonoValueEmpty(mv)) return nullMonoValue;

        for (unsigned int i = 2; i< path.size(); i++) {
            MonoObject* mo = mv.value.obj.o;
            mv = (*mo)[path[i]];
            if (IsMonoValueEmpty(mv)) {
                delete mo;
                return nullMonoValue;
            }

            delete mo;
        }
        return mv;
    }
    #define GETOBJECT(...) getObject(__VA_ARGS__, m_mirrorData->monoImage)

    MonoValue getService(const std::string& name, MonoImage *monoImage) {
        MonoValue dynServices = getObject({"HearthstoneServices", "s_dynamicServices"}, monoImage);
        MonoValue staticServices = getObject({"HearthstoneServices", "s_services"}, monoImage);

        MonoValue ret = nullMonoValue;
        
        MonoValue hsServices;
        if (!IsMonoValueEmpty(dynServices)) {
            hsServices = dynServices;
        } else {
            hsServices = staticServices;
        }

        MonoValue serviceItems = getObject(hsServices, {"m_services", "_items"});
        
        for (unsigned int i = 0; i< serviceItems.arrsize; i++) {
            MonoValue mv = serviceItems[i];
            MonoObject* mo = serviceItems[i].value.obj.o;
            
            if (IsMonoValueEmpty(mv)) {
                continue;
            }

            std::u16string n = (*mo)["<ServiceTypeName>k__BackingField"].str;
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
            std::string n2 = convert.to_bytes(n);

            if (n2 == name) {
                ret = getObject(mv, {"<Service>k__BackingField"});
                break;
            }
        }
        
        DeleteMonoValue(dynServices);
        DeleteMonoValue(staticServices);
        DeleteMonoValue(serviceItems);
        
        return ret;
    }

    #define GETSERVICE(...) getService(__VA_ARGS__, m_mirrorData->monoImage)


    /** Helper to get an int */
    int getInt(const HMObjectPath& path, MonoImage* monoImage) {
        MonoValue mv = getObject(path, monoImage);
        if (IsMonoValueEmpty(mv)) return 0;
        int value = mv.value.i32;

        DeleteMonoValue(mv);
        return value;
    }

    int getInt(const MonoValue& from, const HMObjectPath& path) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return 0;
        int value = mv.value.i32;

        DeleteMonoValue(mv);
        return value;
    }
    #define GETINT(...) getInt(__VA_ARGS__, m_mirrorData->monoImage)

    /** Helper to get a long */
    long getLong(const HMObjectPath& path, MonoImage* monoImage) {
        MonoValue mv = getObject(path, monoImage);
        if (IsMonoValueEmpty(mv)) return 0;
        long value = mv.value.i64;

        DeleteMonoValue(mv);
        return value;
    }

    long getLong(const MonoValue& from, const HMObjectPath& path) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return 0;
        long value = mv.value.i64;

        DeleteMonoValue(mv);
        return value;
    }

    #define GETLONG(...) getLong(__VA_ARGS__, m_mirrorData->monoImage)

    /** Helper to get a bool */
    bool getBool(const HMObjectPath& path, MonoImage* monoImage, bool defaultValue = false) {
        MonoValue mv = getObject(path, monoImage);
        if (IsMonoValueEmpty(mv)) return defaultValue;
        bool value = mv.value.b;

        DeleteMonoValue(mv);
        return value;
    }

    bool getBool(const MonoValue& from, const HMObjectPath& path, bool defaultValue = false) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return defaultValue;
        bool value = mv.value.b;

        DeleteMonoValue(mv);
        return value;
    }

    #define GETBOOL(...) getBool(__VA_ARGS__, m_mirrorData->monoImage)

    #define getAsLong(to, from, key) {\
        MonoValue mv = ((*from)[key]);\
        to = mv.value.i64;\
        DeleteMonoValue(mv);\
    }

    #define getAsBool(to, from, key) {\
        MonoValue mv = ((*from)[key]);\
        to = mv.value.b;\
        DeleteMonoValue(mv);\
    }

    #define getAsInt(to, from, key) {\
        MonoValue mv = ((*from)[key]);\
        to = mv.value.i32;\
        DeleteMonoValue(mv);\
    }

    #define getAsString(to, from, key) {\
        MonoValue mv = ((*from)[key]);\
        to = mv.str;\
        DeleteMonoValue(mv);\
    }

    Deck getDeck(MonoObject* deckObj) {
        Deck deck;

        getAsLong(deck.id, deckObj, "ID");
        getAsString(deck.name, deckObj, "m_name");
        getAsString(deck.hero, deckObj, "HeroCardID");
        getAsBool(deck.isWild, deckObj, "m_isWild");
        getAsInt(deck.type, deckObj, "Type");
        getAsInt(deck.seasonId, deckObj, "SeasonId");
        getAsInt(deck.cardBackId, deckObj, "CardBackID");
        getAsInt(deck.heroPremium, deckObj, "HeroPremium");

        MonoValue _cardList = (*deckObj)["m_slots"];
        if (IsMonoValueEmpty(_cardList)) return deck;

        MonoObject *cardList = _cardList.value.obj.o;

        MonoValue cards = (*cardList)["_items"];
        MonoValue sizemv = (*cardList)["_size"];
        if (IsMonoValueEmpty(cards) || IsMonoValueEmpty(sizemv)) {
            DeleteMonoValue(cards);
            DeleteMonoValue(sizemv);
            DeleteMonoValue(_cardList);
            return deck;
        }
        int size = sizemv.value.i32;
        for (int i = 0; i < size; i++) {
            MonoObject *card = cards[i].value.obj.o;

            std::u16string name;
            getAsString(name, card, "m_cardId");
            MonoValue counts = ((*card)["m_count"]);
            if (IsMonoValueEmpty(counts)) {
                DeleteMonoValue(counts);
                continue;
            }
            MonoObject *items_str = counts.value.obj.o;
            MonoValue items = ((*items_str)["_items"]);
			if (IsMonoValueEmpty(items)) {
                DeleteMonoValue(counts);
                DeleteMonoValue(items);
				continue;
			}
            int count = std::min(items[0].value.i32, 30) + std::min(items[1].value.i32, 30);

            auto iterator = find_if(deck.cards.begin(), deck.cards.end(),
                                    [&name](const Card& obj) { return obj.id == name; });
            if (iterator != deck.cards.end()) {
                auto index = std::distance(deck.cards.begin(), iterator);
                Card c = deck.cards[index];
                c.count += count;
                deck.cards[index] = c;
            } else {
                Card c = Card(name, count, false);
                deck.cards.push_back(c);
            }
            DeleteMonoValue(items);
            DeleteMonoValue(counts);
        }

        DeleteMonoValue(cards);
        DeleteMonoValue(sizemv);
        DeleteMonoValue(_cardList);

        return deck;
    }

    TemplateDeck getTemplateDeck(MonoObject* deckObj) {
        TemplateDeck deck;

        getAsLong(deck.id, deckObj, "m_id");
        getAsString(deck.title, deckObj,"m_title");
        getAsInt(deck.sortOrder, deckObj, "m_sortOrder");
        getAsInt(deck.clazz, deckObj, "m_class");

        MonoValue mv = (*deckObj)["m_cardIds"];
        MonoObject *cards = mv.value.obj.o;
        MonoValue keys = (*cards)["keySlots"];
        MonoValue values = (*cards)["valueSlots"];

        int count;
        getAsInt(count, cards, "count")
        for (unsigned int i = 0; i < count; i++) {
            Card c = Card(keys[i].str, values[i].value.i32, false);
            deck.cards.push_back(c);
        }

        DeleteMonoValue(keys);
        DeleteMonoValue(values);
        DeleteMonoValue(mv);

        return deck;
    }

    int getKeyIndex(MonoObject* object, int key) {
        MonoValue keys = (*object)["keySlots"];
        if (IsMonoValueEmpty(keys)) {
            return -1;
        }
        for (auto i = 0; i < keys.arrsize; i++) {
            MonoStruct* keyObject = keys[i].value.obj.s;
            MonoValue mv = (*keyObject)["value__"];
            if (mv.value.i32 == key) {
                DeleteMonoValue(mv);
                DeleteMonoValue(keys);
                return i;
            }
            DeleteMonoValue(mv);
        }
        DeleteMonoValue(keys);
        return -1;
    }

#pragma mark - Mirror functions

    BattleTag Mirror::getBattleTag() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mv = GETOBJECT({"BnetPresenceMgr","s_instance","m_myPlayer","m_account","m_battleTag"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("Bnet manager can't be found");

        MonoObject* m_battleTag = mv.value.obj.o;

        BattleTag result;
        getAsString(result.name, m_battleTag, "m_name");
        getAsInt(result.number, m_battleTag, "m_number");

        DeleteMonoValue(mv);

        return result;
    }

    AccountId Mirror::getAccountId() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mv = GETOBJECT({"BnetPresenceMgr","s_instance","m_myGameAccountId"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("BNet presence manager can't be found");

        MonoObject* m_accountId = mv.value.obj.o;
        AccountId account;
        getAsLong(account.lo, m_accountId, "m_lo");
        getAsLong(account.hi, m_accountId, "m_hi");

        DeleteMonoValue(mv);
        return account;
    }

    InternalGameServerInfo Mirror::getGameServerInfo() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mv = getObject(GETSERVICE("Network"), {"m_state", "<LastGameServerInfo>k__BackingField"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("Game server info can't be found");
        MonoObject* m_serverInfo = mv.value.obj.o;

        InternalGameServerInfo result;
        getAsString(result.address, m_serverInfo, "<Address>k__BackingField");
        getAsString(result.auroraPassword, m_serverInfo, "<AuroraPassword>k__BackingField");
        getAsLong(result.clientHandle, m_serverInfo, "<ClientHandle>k__BackingField");
        getAsInt(result.gameHandle, m_serverInfo, "<GameHandle>k__BackingField");
        getAsInt(result.mission, m_serverInfo, "<Mission>k__BackingField");
        getAsInt(result.port, m_serverInfo, "<Port>k__BackingField");
        getAsBool(result.resumable, m_serverInfo, "<Resumable>k__BackingField");
        getAsBool(result.spectatorMode, m_serverInfo, "<SpectatorMode>k__BackingField");
        getAsString(result.spectatorPassword, m_serverInfo, "<SpectatorPassword>k__BackingField");
        getAsString(result.version, m_serverInfo, "<Version>k__BackingField");

        DeleteMonoValue(mv);
        return result;
    }

    int Mirror::getGameType() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return getInt(GETSERVICE("GameMgr"), {"m_gameType"});
    }

    int Mirror::getFormat() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return getInt(GETSERVICE("GameMgr"), {"m_formatType"});
    }

    bool Mirror::isSpectating() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return getBool(GETSERVICE("GameMgr"), {"m_spectator"});
    }

    InternalMatchInfo Mirror::getMatchInfo() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        InternalMatchInfo matchInfo;

        MonoValue gameState = GETOBJECT({"GameState","s_instance"});
        MonoValue netCacheValues = getObject(GETSERVICE("NetCache"), {"m_netCache","valueSlots"});
        if (!IsMonoValueEmpty(gameState)) {
            MonoValue playerIds = getObject(gameState, {"m_playerMap","keySlots"});
            MonoValue players = getObject(gameState, {"m_playerMap","valueSlots"});

            if (!IsMonoValueEmpty(players) && !IsMonoValueEmpty(playerIds)
                && IsMonoValueArray(players) && IsMonoValueArray(playerIds)) {

                for (unsigned int i=0; i < playerIds.arrsize; i++) {
                    MonoValue mv = players[i];
                    if (IsMonoValueEmpty(mv)) continue;
                    MonoObject* inst = mv.value.obj.o;

                    MonoClass* instclass = inst->getClass();
                    std::string icname = instclass->getName();
                    delete instclass;

                    if (icname != "Player") {
                        DeleteMonoValue(mv);
                        continue;
                    }

                    MonoValue _medalInfo = (*inst)["m_medalInfo"];
                    MonoObject *medalInfo = _medalInfo.value.obj.o;

                    int sRank = 0;
                    int sLegendRank = 0;
                    int wRank = 0;
                    int wLegendRank = 0;

                    // spectated games have no medalinfo
                    if (medalInfo != NULL) {

                        MonoValue _sMedalInfo = (*medalInfo)["m_currMedalInfo"];
                        if (!IsMonoValueEmpty(_sMedalInfo)) {
                            MonoObject *sMedalInfo = _sMedalInfo.value.obj.o;
                            sRank = getRank(sMedalInfo);
                            MonoValue legendRank = (*sMedalInfo)["legendIndex"];
                            if (!IsMonoValueEmpty(legendRank)) {
                                sLegendRank = legendRank.value.i32;
                                DeleteMonoValue(legendRank);
                            }

                            DeleteMonoValue(_sMedalInfo);
                        }

                        MonoValue _wMedalInfo = (*medalInfo)["m_currWildMedalInfo"];
                        if (!IsMonoValueEmpty(_wMedalInfo)) {
                            MonoObject *wMedalInfo = _wMedalInfo.value.obj.o;
                            wRank = getRank(wMedalInfo);
                            MonoValue legendRank = (*wMedalInfo)["legendIndex"];
                            if (!IsMonoValueEmpty(legendRank)) {
                                wLegendRank = legendRank.value.i32;
                                DeleteMonoValue(legendRank);
                            }

                            DeleteMonoValue(_wMedalInfo);
                        }
                    }
                    std::u16string name;
                    getAsString(name, inst, "m_name");
                    if (name.empty()) {
                        printf("[Hearthmirror] - Found a player with an empty name (Invisible?)\n");
                        name = u"Your Opponent";
                    }

                    int cardBack;
                    getAsInt(cardBack, inst, "m_cardBackId");
                    int id;
                    id = playerIds[i].value.i32;

                    int side;
                    getAsInt(side, inst, "m_side");
                    if (side == 1) {
                        int sStars = 0;
                        int wStars = 0;

                        MonoValue netCacheMedalInfo;
                        for (unsigned int i=0; i< netCacheValues.arrsize; i++) {
                            MonoValue netCache = netCacheValues[i];
                            MonoObject* net = netCache.value.obj.o;
                            if (net == NULL) {
                                continue;
                            }
                            MonoClass* instclass = net->getClass();
                            std::string icname = instclass->getName();
                            delete instclass;

                            if (icname != "NetCacheMedalInfo") {
                                continue;
                            }

                            netCacheMedalInfo = netCache;
                            break;
                        }
                        if (!IsMonoValueEmpty(netCacheMedalInfo)) {
                            MonoObject *net = netCacheMedalInfo.value.obj.o;
                            if (net != NULL) {
                                MonoValue vm = (*net)["<Standard>k__BackingField"];
                                if (!IsMonoValueEmpty(vm)) {
                                    MonoObject* stars = vm.value.obj.o;
                                    getAsInt(sStars, stars, "<Stars>k__BackingField");
                                    DeleteMonoValue(vm);
                                }
                                vm = (*net)["<Wild>k__BackingField"];
                                if (!IsMonoValueEmpty(vm)) {
                                    MonoObject* stars = vm.value.obj.o;
                                    getAsInt(wStars, stars, "<Stars>k__BackingField");
                                    DeleteMonoValue(vm);
                                }
                            }
                        }

                        matchInfo.localPlayer.name = name;
                        matchInfo.localPlayer.id = id;
                        matchInfo.localPlayer.standardRank = sRank;
                        matchInfo.localPlayer.standardLegendRank = sLegendRank;
                        matchInfo.localPlayer.standardStars = sStars;
                        matchInfo.localPlayer.wildRank = wRank;
                        matchInfo.localPlayer.wildLegendRank = wLegendRank;
                        matchInfo.localPlayer.wildStars = wStars;
                        matchInfo.localPlayer.cardBackId = cardBack;
                    } else {
                        matchInfo.opposingPlayer.name = name;
                        matchInfo.opposingPlayer.id = id;
                        matchInfo.opposingPlayer.standardRank = sRank;
                        matchInfo.opposingPlayer.standardLegendRank = sLegendRank;
                        matchInfo.opposingPlayer.standardStars = 0;
                        matchInfo.opposingPlayer.wildRank = wRank;
                        matchInfo.opposingPlayer.wildLegendRank = wLegendRank;
                        matchInfo.opposingPlayer.wildStars = 0;
                        matchInfo.opposingPlayer.cardBackId = cardBack;
                    }

                    DeleteMonoValue(_medalInfo);
                }

                DeleteMonoValue(playerIds);
                DeleteMonoValue(players);
            }
            DeleteMonoValue(gameState);
        }

        MonoValue _gameMgr = GETSERVICE("GameMgr");
        if (!IsMonoValueEmpty(_gameMgr)) {
            MonoObject *gameMgr = _gameMgr.value.obj.o;
            if (gameMgr != NULL) {
                matchInfo.missionId = getInt(_gameMgr, {"m_missionId"});
                matchInfo.gameType = getInt(_gameMgr, {"m_gameType"});
                matchInfo.formatType = getInt(_gameMgr, {"m_formatType"});

                int brawlGameTypes[] = {16, 17, 18};
                size_t size = sizeof(brawlGameTypes) / sizeof(int);
                int *end = brawlGameTypes + size;
                int *found = std::find(brawlGameTypes, end, 0);
                if (found != end) {
                    MonoValue mission = getCurrentBrawlMission(m_mirrorData->monoImage);
                    matchInfo.brawlSeasonId = getInt(mission, {"<tavernBrawlSpec>k__BackingField", "<GameContentSeason>k__BackingField", "<SeasonId>k__BackingField"});
                    DeleteMonoValue(mission);
                }
            }

            DeleteMonoValue(_gameMgr);
        }

        for (unsigned int i=0; i< netCacheValues.arrsize; i++) {
            MonoValue netCache = netCacheValues[i];
            if (IsMonoValueEmpty(netCache)) continue;
            MonoObject* net = netCache.value.obj.o;
            MonoClass* instclass = net->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "NetCacheRewardProgress") {
                continue;
            }

            getAsInt(matchInfo.rankedSeasonId, net, "<Season>k__BackingField");
            break;
        }

        DeleteMonoValue(netCacheValues);

        return matchInfo;
    }

    MonoValue getCurrentBrawlMission(MonoImage* monoImage) {
        if (!monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue missions = getObject(getService("TavernBrawlManager", monoImage), {"m_missions"});
        if (IsMonoValueEmpty(missions) || !IsMonoValueArray(missions)) { return NULL; }
        MonoValue record(0);
        for (unsigned int i=0; i< missions.arrsize; i++) {
            MonoValue mission = missions[i];
            if (IsMonoValueEmpty(mission)) continue;
            MonoObject* _mission = mission.value.obj.o;
            MonoClass* instclass = _mission->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "TavernBrawlMission") {
                continue;
            }

            return mission;
        }

        return nullMonoValue;
    }

    BrawlInfo Mirror::getBrawlInfo() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mission = getCurrentBrawlMission(m_mirrorData->monoImage);
        if (IsMonoValueEmpty(mission)) throw std::domain_error("Current brawl not found");

        BrawlInfo result;
        MonoObject *_mission = mission.value.obj.o;
        if (_mission == NULL) {
            DeleteMonoValue(mission);
            throw std::domain_error("Current brawl not found");
        }
        MonoValue tavernBrawlSpec = getObject(mission, {"tavernBrawlSpec","<GameContentSeason>k__BackingField"});
        if (IsMonoValueEmpty(tavernBrawlSpec)) {
            DeleteMonoValue(mission);
            throw std::domain_error("Current brawl not found");
        }
        MonoObject *_tavernBrawlSpec = tavernBrawlSpec.value.obj.o;
        getAsInt(result.maxWins, _tavernBrawlSpec, "_MaxWins");
        getAsInt(result.maxLosses, _tavernBrawlSpec, "_MaxLosses");

        MonoValue records = getObject(GETSERVICE("TavernBrawlManager"), {"m_playerRecords"});
        if (IsMonoValueEmpty(records) || !IsMonoValueArray(records))
            throw std::domain_error("Brawl manager can't be found");

        MonoValue record(0);
        for (unsigned int i=0; i< records.arrsize; i++) {
            MonoValue r = records[i];
            if (IsMonoValueEmpty(r)) continue;
            MonoObject* _record = r.value.obj.o;
            MonoClass* instclass = _record->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "TavernBrawlPlayerRecord") {
                continue;
            }

            record = r;
            break;
        }
        if (IsMonoValueEmpty(record)) throw std::domain_error("Can't get record");

        result.gamesPlayed = getInt(record, {"_GamesPlayed"});
        result.winStreak = getInt(record, {"_WinStreak"});

        MonoObject *_record = record.value.obj.o;
        if (_record == NULL) {
            DeleteMonoValue(record);
            DeleteMonoValue(records);
            throw std::domain_error("Can't get record");
        }

        getAsInt(result.gamesPlayed, _record, "_GamesPlayed");
        getAsInt(result.winStreak, _record, "_WinStreak");
        result.isSessionBased = result.maxWins > 0 || result.maxLosses > 0;
        if (result.isSessionBased) {
            if (!((*_record)["HasSession"]).value.b) {
                DeleteMonoValue(mission);
                DeleteMonoValue(records);
                DeleteMonoValue(record);
                return result;
            }

            MonoValue session = getObject(record, {"_Session"});
            result.wins = getInt(session, {"<Wins>k__BackingField"});
            result.losses = getInt(session, {"<Losses>k__BackingField"});
            DeleteMonoValue(session);
        } else {
            result.wins = getInt(record, {"<GamesWon>k__BackingField"});
            result.losses = result.gamesPlayed - result.wins;
        }

        DeleteMonoValue(mission);
        DeleteMonoValue(records);
        return result;
    }

    DungeonInfo Mirror::getDungeonInfo() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");
        MonoValue dataMap = GETOBJECT({"GameSaveDataManager","s_instance","m_gameSaveDataMapByKey"});
        if (IsMonoValueEmpty(dataMap)) {
            throw std::domain_error("GameSaveDataManager manager can't be found");
        }
        MonoObject *_dataMap = dataMap.value.obj.o;
        int index = getKeyIndex(_dataMap, ADVENTURE_DATA_LOOT);
        if (index == -1) {
            throw std::domain_error("Key ADVENTURE_DATA_LOOT not found");
        }

        MonoValue valueSlots = (*_dataMap)["valueSlots"]; // D
        MonoValue subMap = valueSlots[index];
        if (IsMonoValueEmpty(subMap)) {
            throw std::domain_error("SubMap not found");
        }

        DungeonInfo dungeonInfo;
        dungeonInfo.load(subMap.value.obj.o);

        DeleteMonoValue(valueSlots);
        DeleteMonoValue(dataMap);
        return dungeonInfo;
    }

    Deck Mirror::getEditedDeck() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue taggedDecks = GETOBJECT({"CollectionManager","s_instance","m_taggedDecks"});
        if (IsMonoValueEmpty(taggedDecks)) {
            throw std::domain_error("Collection manager can't be found");
        }
        MonoObject *_taggedDecks = taggedDecks.value.obj.o;
        MonoValue tags = (*_taggedDecks)["keySlots"];
        MonoValue decks = (*_taggedDecks)["valueSlots"];
        if (IsMonoValueEmpty(tags) || IsMonoValueEmpty(decks)) {
            throw std::domain_error("No edited deck ?");
        }
        for (unsigned int i = 0; i < tags.arrsize; i++) {
            MonoValue tag = tags[i];
            MonoValue deck = decks[i];
            if (IsMonoValueEmpty(tag) || IsMonoValueEmpty(deck)) continue;

            MonoStruct *_tag = tag.value.obj.s;
            try {
                int v;
                getAsInt(v, _tag, "value__");
                if (v == 0) {
                    Deck ddeck = getDeck(deck.value.obj.o);
                    DeleteMonoValue(tags);
                    DeleteMonoValue(decks);
                    DeleteMonoValue(taggedDecks);
                    return ddeck;
                }
            } catch (std::exception& ex) {
                continue;
            }
        }

        DeleteMonoValue(tags);
        DeleteMonoValue(decks);
        DeleteMonoValue(taggedDecks);
        throw std::domain_error("No edited deck ?");
    }

    std::vector<Deck> Mirror::getDecks() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue values = GETOBJECT({"CollectionManager","s_instance","m_decks","valueSlots"});
        if (IsMonoValueEmpty(values) || !IsMonoValueArray(values)) {
            throw std::domain_error("Collection manager can't be found");
        }

        std::vector<Deck> result;

        for (unsigned int i=0; i< values.arrsize; i++) {
            MonoValue mv = values[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "CollectionDeck") {
                continue;
            }

            Deck deck = getDeck(inst);
            // count cards
            int sum = std::accumulate(begin(deck.cards), end(deck.cards), 0,
                              [](const int& x, const Card& y) { return x + y.count; });
            if (deck.cards.size() == 0 || sum != 30) {
                continue;
            }

            // don't add the same deck multiple times
            auto iterator = std::find_if(result.begin(), result.end(),
                                    [&deck](const Deck& obj) { return obj.id == deck.id; });
            if (iterator == result.end()) {
                result.push_back(deck);
            }
        }

        DeleteMonoValue(values);
        return result;
    }

    std::vector<TemplateDeck> Mirror::getTemplateDecks() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue values = GETOBJECT({"CollectionManager","s_instance","m_templateDecks","valueSlots"});
        if (IsMonoValueEmpty(values) || !IsMonoValueArray(values)) {
            throw std::domain_error("Collection manager can't be found");
        }

        std::vector<TemplateDeck> result;

        for (unsigned int i=0; i< values.arrsize; i++) {
            MonoValue mv = values[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;

            MonoValue values2 = (*inst)["_items"];
            for (unsigned int i=0; i< (*inst)["_size"].value.i32; i++) {
                MonoValue mv2 = values2[i];
                if (IsMonoValueEmpty(mv2)) continue;

                MonoObject* inst2 = mv2.value.obj.o;
                MonoClass* instclass = inst2->getClass();
                std::string icname = instclass->getName();
                delete instclass;

                if (icname != "TemplateDeck") {
                    continue;
                }

                TemplateDeck deck = getTemplateDeck(inst2);
                result.push_back(deck);
            }
            DeleteMonoValue(values2);
        }

        DeleteMonoValue(values);
        return result;
    }

    long Mirror::getSelectedDeckInMenu() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETLONG({"DeckPickerTrayDisplay","s_instance","m_selectedCustomDeckBox","m_deckID"});
    }

    std::vector<Card> Mirror::getPackCards() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue cards = GETOBJECT({"PackOpening","s_instance","m_director","m_hiddenCards","_items"});
        if (IsMonoValueEmpty(cards) || !IsMonoValueArray(cards)) {
            throw std::domain_error("Pack opening informations can't be found");
        }

        std::vector<Card> result;
        for (unsigned int i = 0; i < cards.arrsize; i++) {
            MonoValue mv = cards[i];
            if (IsMonoValueEmpty(mv)) continue;

            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "PackOpeningCard") continue;
            MonoValue def = getObject(mv, {"m_boosterCard","<Def>k__BackingField"});
            if (IsMonoValueEmpty(def)) continue;

            MonoObject *_def = def.value.obj.o;
            MonoValue namemv = (*_def)["<Name>k__BackingField"];
            MonoValue premiummv = (*_def)["<Premium>k__BackingField"];
            if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                continue;
            }

            std::u16string name = namemv.str;
            bool premium = premiummv.value.i32 > 0;
            result.push_back(Card(name, 1, premium));
        }

        return result;
    }

    ArenaInfo Mirror::getArenaDeck() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        ArenaInfo result;

        MonoValue _draftManager = GETSERVICE("DraftManager");
        if (IsMonoValueEmpty(_draftManager)) throw std::domain_error("Draft manager can't be found");
        MonoObject* draftManager = _draftManager.value.obj.o;

        MonoValue draftDeck = (*draftManager)["m_draftDeck"];
        if (IsMonoValueEmpty(draftDeck)) {
            DeleteMonoValue(_draftManager);
            throw std::domain_error("Draft deck can't be found");
        }

        MonoObject* inst = draftDeck.value.obj.o;
        if (inst == NULL) {
            throw std::domain_error("Draft deck pointer is null");
        }
        Deck deck = getDeck(inst);
        result.deck = deck;

        MonoValue wins = (*draftManager)["m_wins"];
        MonoValue losses = (*draftManager)["m_losses"];
        MonoValue currentSlot = (*draftManager)["m_currentSlot"];

        result.wins = wins.value.i32;
        result.losses = losses.value.i32;
        result.currentSlot = currentSlot.value.i32;
        MonoValue chest = (*draftManager)["m_chest"];
        if (!IsMonoValueEmpty(chest)) {
            MonoObject *_chest = chest.value.obj.o;
            MonoValue rewards = (*_chest)["<Rewards>k__BackingField"];
            if (IsMonoValueEmpty(rewards)) {
                DeleteMonoValue(chest);
            } else {
                MonoObject *_rewards = rewards.value.obj.o;
                MonoValue items = (*_rewards)["_items"];
                if (!IsMonoValueEmpty(items) && IsMonoValueArray(items)) {
                    result.rewards = parseRewards(items);
                    DeleteMonoValue(items);
                }

                DeleteMonoValue(rewards);
            }
            DeleteMonoValue(chest);
        }

        DeleteMonoValue(draftDeck);
        DeleteMonoValue(_draftManager);
        return result;
    }

    std::vector<Card> Mirror::getArenaDraftChoices() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        std::vector<Card> result;
        MonoValue values = GETOBJECT({"DraftDisplay","s_instance","m_choices"});
        if (IsMonoValueEmpty(values)) throw std::domain_error("Draft choices can't be found");

        MonoObject *stacks = values.value.obj.o;
        MonoValue choices = (*stacks)["_items"];
        MonoValue sizemv = (*stacks)["_size"];

        int size = sizemv.value.i32;
        for (unsigned int i = 0; i < size; i++) {
            MonoValue mv = getObject(choices[i], {"m_actor","m_entityDef","m_cardIdInternal"});
            if (IsMonoValueEmpty(mv)) continue;

            result.push_back(Card(mv.str, 1, false));
        }

        DeleteMonoValue(values);
        return result;
    }

    std::vector<RewardData*> parseRewards(MonoValue items) {
        std::vector<RewardData*> result;

        for (unsigned int i = 0; i < items.arrsize; i++) {
            MonoValue mv = items[i];
            if (IsMonoValueEmpty(mv)) continue;

            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname == "ArcaneDustRewardData") {
                ArcaneDustRewardData *data = new ArcaneDustRewardData();
                data->type = ARCANE_DUST;
                getAsInt(data->amount, inst, "<Amount>k__BackingField");
                result.push_back(data);
            } else if (icname == "BoosterPackRewardData") {
                BoosterPackRewardData *data = new BoosterPackRewardData();
                getAsInt(data->id, inst, "<Id>k__BackingField");
                data->type = BOOSTER_PACK;
                getAsInt(data->count, inst, "<Count>k__BackingField");
                result.push_back(data);
            } else if (icname == "CardRewardData") {
                CardRewardData *data = new CardRewardData();
                getAsString(data->id, inst, "<CardID>k__BackingField");
                getAsInt(data->count, inst, "<Count>k__BackingField");
                int premium;
                getAsInt(premium, inst, "<Premium>k__BackingField");
                data->premium = premium > 0;
                data->type = CARD;
                result.push_back(data);
            } else if (icname == "CardBackRewardData") {
                CardBackRewardData *data = new CardBackRewardData();
                getAsInt(data->id, inst, "<CardBackID>k__BackingField");
                data->type = CARD_BACK;
                result.push_back(data);
            } else if (icname == "ForgeTicketRewardData") {
                ForgeTicketRewardData *data = new ForgeTicketRewardData();
                getAsInt(data->quantity, inst, "<Quantity>k__BackingField");
                data->type = FORGE_TICKET;
                result.push_back(data);
            } else if (icname == "GoldRewardData") {
                GoldRewardData *data = new GoldRewardData();
                getAsInt(data->amount, inst, "<Amount>k__BackingField");
                data->type = GOLD;
                result.push_back(data);
            } else if (icname == "MountRewardData") {
                MountRewardData *data = new MountRewardData();
                getAsInt(data->mountType, inst, "<Mount>k__BackingField");
                data->type = MOUNT;
                result.push_back(data);
            }
        }

        return result;
    }

    Collection Mirror::getCollection() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue valueSlots = getObject(GETSERVICE("NetCache"), {"m_netCache","valueSlots"});
        if (IsMonoValueEmpty(valueSlots) || !IsMonoValueArray(valueSlots)) {
            throw std::domain_error("Net cache can't be found");
        }

        Collection result;

        for (unsigned int i=0; i < valueSlots.arrsize; i++) {
            MonoValue mv = valueSlots[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname == "NetCacheCollection") {
                MonoValue stacksmv = (*inst)["<Stacks>k__BackingField"];
                if (IsMonoValueEmpty(stacksmv)) break;

                MonoObject* stacks = stacksmv.value.obj.o;
                MonoValue itemsmv = (*stacks)["_items"];
                MonoValue sizemv = (*stacks)["_size"];
                if (IsMonoValueEmpty(itemsmv) || IsMonoValueEmpty(sizemv)) break;
                int size = sizemv.value.i32;
                for (int i=0; i< size; i++) { // or itemsmv.arrsize?
                    MonoValue stackmv = itemsmv.value.arr[i];
                    if (IsMonoValueEmpty(stackmv)) continue;
                    MonoObject* stack = stackmv.value.obj.o;

                    MonoValue countmv = (*stack)["<Count>k__BackingField"];
                    if (IsMonoValueEmpty(countmv)) {
                        continue;
                    }
                    int count = countmv.value.i32;
                    DeleteMonoValue(countmv);

                    MonoValue defmv = (*stack)["<Def>k__BackingField"];
                    if (IsMonoValueEmpty(defmv)) {
                        continue;
                    }
                    MonoObject* def = defmv.value.obj.o;

                    MonoValue namemv = (*def)["<Name>k__BackingField"];
                    MonoValue premiummv = (*def)["<Premium>k__BackingField"];
                    if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                        DeleteMonoValue(defmv);
                        continue;
                    }

                    std::u16string name = namemv.str;
                    bool premium = premiummv.value.b;
                    result.cards.push_back(Card(name,count,premium));

                    DeleteMonoValue(defmv);
                    DeleteMonoValue(namemv);
                    DeleteMonoValue(premiummv);
                }
                DeleteMonoValue(sizemv);
                DeleteMonoValue(itemsmv);
                DeleteMonoValue(stacksmv);

            } else if (icname == "NetCacheCardBacks") {
                MonoValue cardBackSlots = getObject(mv, {"<CardBacks>k__BackingField", "slots"});
                if (IsMonoValueEmpty(cardBackSlots) || !IsMonoValueArray(cardBackSlots)) {
                    throw std::domain_error("Invalid cardback object");
                }
                std::unordered_set<int> cardbacks;
                for (unsigned int j=0; j < cardBackSlots.arrsize; j++) {
                    MonoValue cardback = cardBackSlots[j];
                    cardbacks.insert(cardback.value.i32);
                }
                DeleteMonoValue(cardBackSlots);
                result.cardbacks.insert(result.cardbacks.end(), cardbacks.begin(), cardbacks.end());

                MonoValue defaultCardbackMv = getObject(mv, {"<DefaultCardBack>k__BackingField"});
                result.favoriteCardBack = defaultCardbackMv.value.i32;
                DeleteMonoValue(defaultCardbackMv);
            } else if (icname == "NetCacheArcaneDustBalance") {
                MonoValue dustmv = getObject(mv, {"<Balance>k__BackingField"});
                result.dust = dustmv.value.i32;
                DeleteMonoValue(dustmv);
            } else if (icname == "NetCacheGoldBalance") {
                MonoValue goldmv = getObject(mv, {"<CappedBalance>k__BackingField"});
                auto gold = goldmv.value.i32;
                DeleteMonoValue(goldmv);
                MonoValue bonusgoldmv = getObject(mv, {"<BonusBalance>k__BackingField"});
                gold += bonusgoldmv.value.i32;
                DeleteMonoValue(bonusgoldmv);
                result.gold = gold;
            } else if (icname == "NetCacheFavoriteHeroes") {
                MonoValue keysMonoValue = getObject(mv, {"<FavoriteHeroes>k__BackingField", "keySlots"});
                if (IsMonoValueEmpty(keysMonoValue) || !IsMonoValueArray(keysMonoValue)) {
                    throw std::domain_error("Invalid favorite heroes key object");
                }

                MonoValue valuesMonoValue = getObject(mv, {"<FavoriteHeroes>k__BackingField", "valueSlots"});
                if (IsMonoValueEmpty(valuesMonoValue) || !IsMonoValueArray(valuesMonoValue)) {
                    throw std::domain_error("Invalid favorite heroes value object");
                }

                for (unsigned int k=0; k < keysMonoValue.arrsize; k++) {
                    MonoValue heroMonoValue = valuesMonoValue[k];
                    if (IsMonoValueEmpty(heroMonoValue)) continue;
                    MonoClass* instclass = heroMonoValue.value.obj.o->getClass();
                    std::string classname = instclass->getName();
                    delete instclass;

                    if (classname == "CardDefinition") {
                        std::u16string cardId = getObject(heroMonoValue, {"<Name>k__BackingField"}).str;
                        int premium = getObject(heroMonoValue, {"<Premium>k__BackingField"}).value.i32;

                        auto key = (*keysMonoValue[k].value.obj.s)["value__"];

                        result.favoriteHeroes[int(key.value.i32)] = Card(cardId, 1, premium > 0);

                        DeleteMonoValue(key);
                    }
                }

                DeleteMonoValue(keysMonoValue);
                DeleteMonoValue(valuesMonoValue);
            }
        }

        // free all memory
        DeleteMonoValue(valueSlots);

        return result;
    }

    std::vector<HeroLevel> Mirror::getHeroLevels() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue valueSlots = getObject(GETSERVICE("NetCache"), {"m_netCache","valueSlots"});
        if (IsMonoValueEmpty(valueSlots) || !IsMonoValueArray(valueSlots)) {
            throw std::domain_error("Net cache can't be found");
        }

        std::vector<HeroLevel> result;

        for (unsigned int i=0; i< valueSlots.arrsize; i++) {
            MonoValue mv = valueSlots[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;
            if (icname == "NetCacheHeroLevels") {
                MonoValue stacksmv = (*inst)["<Levels>k__BackingField"];
                if (IsMonoValueEmpty(stacksmv)) break;

                MonoObject* stacks = stacksmv.value.obj.o;
                MonoValue itemsmv = (*stacks)["_items"];
                MonoValue sizemv = (*stacks)["_size"];
                if (IsMonoValueEmpty(itemsmv) || IsMonoValueEmpty(sizemv)) break;
                int size = sizemv.value.i32;
                for (int i=0; i< size; i++) { // or itemsmv.arrsize?
                    MonoValue stackmv = itemsmv.value.arr[i];
                    if (IsMonoValueEmpty(stackmv)) continue;
                    MonoObject* stack = stackmv.value.obj.o;

                    MonoValue classmv = (*stack)["<Class>k__BackingField"];
                    if (IsMonoValueEmpty(classmv)) {
                        continue;
                    }
                    int heroclass = classmv.value.i32;
                    DeleteMonoValue(classmv);

                    MonoValue clevelmv = (*stack)["<CurrentLevel>k__BackingField"];
                    if (IsMonoValueEmpty(clevelmv)) {
                        continue;
                    }
                    MonoObject* currentLvl = clevelmv.value.obj.o;

                    MonoValue levelmv = (*currentLvl)["<Level>k__BackingField"];
                    MonoValue maxLevelmv = (*currentLvl)["<MaxLevel>k__BackingField"];
                    MonoValue xpmv = (*currentLvl)["<XP>k__BackingField"];
                    MonoValue maxXpmv = (*currentLvl)["<MaxXP>k__BackingField"];
                    if (IsMonoValueEmpty(levelmv) || IsMonoValueEmpty(maxLevelmv)
                        || IsMonoValueEmpty(xpmv) || IsMonoValueEmpty(maxXpmv)) {
                        DeleteMonoValue(clevelmv);
                        continue;
                    }

                    HeroLevel heroLevel;
                    heroLevel.heroClass = heroclass;
                    heroLevel.level = levelmv.value.i32;
                    heroLevel.maxLevel = maxLevelmv.value.i32;
                    heroLevel.xp = xpmv.value.i64;
                    heroLevel.maxXp = maxXpmv.value.i64;

                    result.push_back(heroLevel);

                    DeleteMonoValue(clevelmv);
                }
                DeleteMonoValue(sizemv);
                DeleteMonoValue(itemsmv);
                DeleteMonoValue(stacksmv);

            }
        }

        // free all memory
        DeleteMonoValue(valueSlots);

        return result;
    }

    bool Mirror::isFriendsListVisible() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"ChatMgr","s_instance","m_friendListFrame"});
    }

    bool Mirror::isGameMenuVisible() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"GameMenu","s_instance","m_isShown"});
    }

    bool Mirror::isOptionsMenuVisible() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"OptionsMenu","s_instance","m_isShown"});
    }

    bool Mirror::isMulligan() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"MulliganManager","s_instance","mulliganChooseBanner"});
    }

    int Mirror::getNumMulliganCards() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"MulliganManager","s_instance","m_startingCards","_size"});
    }

    bool Mirror::isChoosingCard() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"ChoiceCardMgr","s_instance","m_subOptionState"}) ||
                GETINT({"ChoiceCardMgr","s_instance","m_choiceStateMap","count"}) > 0;
    }

    int Mirror::getNumChoiceCards() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"ChoiceCardMgr","s_instance","m_lastShownChoices","_size"});
    }

    bool Mirror::isPlayerEmotesVisible() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"EmoteHandler","s_instance","m_emotesShown"}, m_mirrorData->monoImage);
    }

    bool Mirror::isEnemyEmotesVisible() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"EnemyEmoteHandler","s_instance","m_emotesShown"}, m_mirrorData->monoImage);
    }

    bool Mirror::isInBattlecryEffect() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_isInBattleCryEffect"}, m_mirrorData->monoImage);
    }

    bool Mirror::isDragging() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_dragging"}, m_mirrorData->monoImage);
    }

    bool Mirror::isTargetingHeroPower() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_targettingHeroPower"});
    }

    int Mirror::getBattlecrySourceCardZonePosition() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"InputManager","s_instance","m_battlecrySourceCard","m_zonePosition"});
    }

    bool Mirror::isHoldingCard() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_heldCard"});
    }

    bool Mirror::isTargetReticleActive() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"TargetReticleManager","s_instance","m_isActive"});
    }

    bool Mirror::isEnemyTargeting() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_isEnemyArrow"});
    }

    bool Mirror::isGameOver() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"GameState","s_instance","m_gameOver"});
    }

    bool Mirror::isInMainMenu() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"Box","s_instance","m_state"}) == (int)kBoxStateHubWithDrawer;
    }

    UI_WINDOW Mirror::getShownUiWindowId() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return (UI_WINDOW)GETINT({"ShownUIMgr","s_instance","m_shownUI"});
    }

    SceneMode Mirror::GetCurrentSceneMode() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return (SceneMode)getInt(GETSERVICE("SceneMgr"), {"m_mode"});
    }

    bool Mirror::isPlayerHandZoneUpdatingLayout() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_myHandZone", "m_updatingLayout"});
    }

    bool Mirror::isPlayerPlayZoneUpdatingLayout() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETBOOL({"InputManager","s_instance","m_myPlayZone", "m_updatingLayout"});
    }

    int Mirror::getNumCardsPlayerHand() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"InputManager","s_instance","m_myHandZone","m_cards","_size"});
    }

    int Mirror::getNumCardsPlayerBoard() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"InputManager","s_instance","m_myPlayZone","m_cards","_size"});
    }

    int Mirror::getNavigationHistorySize() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"Navigation","history","_size"});
    }

    int Mirror::getCurrentManaFilter() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"CollectionManagerDisplay","s_instance","m_manaTabManager","m_currentFilterValue"});
    }

    SetFilterItem Mirror::getCurrentSetFilter() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue item = GETOBJECT({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected"});
        if (IsMonoValueEmpty(item)) {
            DeleteMonoValue(item);
            return SetFilterItem();
        }

        DeleteMonoValue(item);
        SetFilterItem result = SetFilterItem();

        result.isAllStandard = GETBOOL({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected","m_isAllStandard"});
        result.isWild = GETBOOL({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected","m_isWild"});
        return result;
    }

    int Mirror::getLastOpenedBoosterId() {
        if (!m_mirrorData->monoImage) throw std::domain_error("Mono image can't be found");

        return GETINT({"PackOpening","s_instance","m_lastOpenedBoosterId"});
    }

    int Mirror::getRank(MonoObject *medalInfo) {
        int leagueId;
        getAsInt(leagueId, medalInfo, "leagueId");
        int starLevel;
        getAsInt(starLevel, medalInfo, "starLevel");

        int rank = 0;

        MonoValue rankMgr = GETOBJECT({"RankMgr", "s_instance"});
        if(IsMonoValueEmpty(rankMgr))
            return rank;

        MonoValue leagueMap = (*rankMgr.value.obj.o)["m_rankConfigByLeagueAndStarLevel"];
        if(IsMonoValueEmpty(leagueMap)) {
            DeleteMonoValue(rankMgr);
            return rank;
        }

        MonoValue keys = (*leagueMap.value.obj.o)["keySlots"];
        MonoValue values = (*leagueMap.value.obj.o)["valueSlots"];

        for (unsigned int i = 0; i < keys.arrsize; i++) {
            if (keys[i].value.i32 != leagueId) {
                continue;
            }
            if (IsMonoValueEmpty(values[i])) {
                continue;
            }
            MonoObject *starLevelMap = values[i].value.obj.o;

            MonoValue keys2 = (*starLevelMap)["keySlots"];
            MonoValue values2 = (*starLevelMap)["valueSlots"];
            for(int j = 0; j < keys2.arrsize; j++)
            {
                if(keys2[j].value.i32 != starLevel)
                    continue;
                
                MonoValue locValues = getObject(values2[j], {"m_medalText", "m_locValues", "_items"});
                
                for (int k = 0; k < locValues.arrsize; k++) {
                    MonoValue value = locValues[k];
                
                    if (IsMonoValueEmpty(value)) continue;
                    
                    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
                    std::string dest = convert.to_bytes(value.str);
                    rank = atoi(dest.c_str());

                    if (rank != 0) {
                        break;
                    }
                }
                    
                DeleteMonoValue(locValues);
                
                if (rank != 0) {
                    break;
                }
            }

            DeleteMonoValue(keys2);
            DeleteMonoValue(values2);

            if (rank != 0) {
                break;
            }
        }

        DeleteMonoValue(keys);
        DeleteMonoValue(values);

        DeleteMonoValue(leagueMap);
        DeleteMonoValue(rankMgr);

        return rank;
    }

    enum GameSaveKeySubkeyId {
        DUNGEON_CRAWL_BOSSES_DEFEATED = 13,
        DUNGEON_CRAWL_DECK_CARD_LIST,
        DUNGEON_CRAWL_DECK_CLASS,
        DUNGEON_CRAWL_PLAYER_PASSIVE_BUFF_LIST = 18,
        DUNGEON_CRAWL_NEXT_BOSS_FIGHT,
        DUNGEON_CRAWL_LOOT_OPTION_A = 23,
        DUNGEON_CRAWL_LOOT_OPTION_B,
        DUNGEON_CRAWL_LOOT_OPTION_C,
        DUNGEON_CRAWL_TREASURE_OPTION,
        DUNGEON_CRAWL_LOOT_HISTORY = 80,
        DUNGEON_CRAWL_IS_RUN_ACTIVE,
        DUNGEON_CRAWL_PALADIN_BOSS_WINS,
        DUNGEON_CRAWL_PALADIN_RUN_WINS,
        DUNGEON_CRAWL_WARRIOR_BOSS_WINS,
        DUNGEON_CRAWL_WARRIOR_RUN_WINS,
        DUNGEON_CRAWL_DRUID_BOSS_WINS,
        DUNGEON_CRAWL_DRUID_RUN_WINS,
        DUNGEON_CRAWL_ROGUE_BOSS_WINS,
        DUNGEON_CRAWL_ROGUE_RUN_WINS,
        DUNGEON_CRAWL_SHAMAN_BOSS_WINS,
        DUNGEON_CRAWL_SHAMAN_RUN_WINS,
        DUNGEON_CRAWL_PRIEST_BOSS_WINS,
        DUNGEON_CRAWL_PRIEST_RUN_WINS,
        DUNGEON_CRAWL_WARLOCK_BOSS_WINS,
        DUNGEON_CRAWL_WARLOCK_RUN_WINS,
        DUNGEON_CRAWL_HUNTER_BOSS_WINS,
        DUNGEON_CRAWL_HUNTER_RUN_WINS,
        DUNGEON_CRAWL_MAGE_BOSS_WINS,
        DUNGEON_CRAWL_MAGE_RUN_WINS,
        DUNGEON_CRAWL_BOSS_LOST_TO,
        DUNGEON_CRAWL_PLAYER_CHOSEN_LOOT,
        DUNGEON_CRAWL_PLAYER_CHOSEN_TREASURE,
        DUNGEON_CRAWL_NEXT_BOSS_HEALTH = 123,
        DUNGEON_CRAWL_HERO_HEALTH = 130,
        DUNGEON_CRAWL_CARDS_ADDED_TO_DECK_MAP = 135
    };

    std::vector<int> getValueAsVector(const GameSaveKeySubkeyId& key, MonoObject *map) {
        std::vector<int> result;
        int subIndex = getKeyIndex(map, key);
        if (subIndex == -1) {
            return result;
        }

        MonoValue valueSlots = (*map)["valueSlots"];
        if (IsMonoValueEmpty(valueSlots)) {
            throw std::runtime_error("Failed to access valueSlots");
        }
        MonoValue value = valueSlots[subIndex];
        MonoValue list = (*value.value.obj.o)["_IntValue"];
        MonoValue sizeMv = (*list.value.obj.o)["_size"];
        int size = sizeMv.value.i32;
        DeleteMonoValue(sizeMv);


        if (size > 0) {
            MonoValue items = (*list.value.obj.o)["_items"];
            for (auto i = 0; i < size; i++) {
                auto item = intValue(items[i]);
                result.push_back((int)item);
            }
            DeleteMonoValue(items);
        }

        DeleteMonoValue(list);
        DeleteMonoValue(valueSlots);

        return result;
    }

    int getValue(const GameSaveKeySubkeyId& key, MonoObject *map) {
        try {
            std::vector<int> values = getValueAsVector(key, map);
            if (values.size() > 0) {
                return values[0];
            }
            return 0;
        } catch (std::runtime_error& err) {
            return 0;
        }
    }

    void DungeonInfo::load(MonoObject *map) {
        this->bossesDefeated = getValueAsVector(DUNGEON_CRAWL_BOSSES_DEFEATED, map);
        this->dbfIds = getValueAsVector(DUNGEON_CRAWL_DECK_CARD_LIST, map);
        this->heroCardClass = getValue(DUNGEON_CRAWL_DECK_CLASS, map);
        this->passiveBuffs = getValueAsVector(DUNGEON_CRAWL_PLAYER_PASSIVE_BUFF_LIST, map);
        this->nextBossDbfId = getValue(DUNGEON_CRAWL_NEXT_BOSS_FIGHT, map);
        this->lootA = getValueAsVector(DUNGEON_CRAWL_LOOT_OPTION_A, map);
        this->lootB = getValueAsVector(DUNGEON_CRAWL_LOOT_OPTION_B, map);
        this->lootC = getValueAsVector(DUNGEON_CRAWL_LOOT_OPTION_C, map);
        this->treasure = getValueAsVector(DUNGEON_CRAWL_TREASURE_OPTION, map);
        this->lootHistory = getValueAsVector(DUNGEON_CRAWL_LOOT_HISTORY, map);
        this->runActive = getValue(DUNGEON_CRAWL_IS_RUN_ACTIVE, map) > 0;
        this->bossesLostTo = getValue(DUNGEON_CRAWL_BOSS_LOST_TO, map);
        this->playerChosenLoot = getValue(DUNGEON_CRAWL_PLAYER_CHOSEN_LOOT, map);
        this->playerChosenTreasure = getValue(DUNGEON_CRAWL_PLAYER_CHOSEN_TREASURE, map);
        this->nextBossHealth = getValue(DUNGEON_CRAWL_NEXT_BOSS_HEALTH, map);
        this->heroHealth = getValue(DUNGEON_CRAWL_HERO_HEALTH, map);
        this->cardsAddedToDeck = getValueAsVector(DUNGEON_CRAWL_CARDS_ADDED_TO_DECK_MAP, map);
    }

} // namespace
