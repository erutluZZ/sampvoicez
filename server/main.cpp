<<<<<<< HEAD
<<<<<<< HEAD
/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/
=======
﻿#include "sv_header.h"

#include "sv_net.h"
#include "sv_pawn.h"
#include "sv_audio.h"

namespace core {
>>>>>>> parent of 40cfcff (upgrade to 3.0)

	static constexpr bool debug_mode = false;

	// Таблица игроков
	static st_player_info players[MAX_PLAYERS] = { 0 };

<<<<<<< HEAD
#include "Pawn.h"
#include "Config.h"
#include "Network.h"
#include "PlayerStore.h"
#include "Worker.h"
=======
=======
﻿#include "sv_header.h"

#include "sv_net.h"
#include "sv_pawn.h"
#include "sv_audio.h"

namespace core {

	static constexpr bool debug_mode = false;

	// Таблица игроков
	static st_player_info players[MAX_PLAYERS] = { 0 };

>>>>>>> parent of 40cfcff (upgrade to 3.0)
	// Списки локальных потоков с автоматическим режимом привязки
	static std::set<audio::streams::stream_dynamic_local_at_point*>							streams_at_point;
	static std::map<uint16_t, std::set<audio::streams::stream_dynamic_local_at_vehicle*>>	streams_at_vehicle;
	static std::map<uint16_t, std::set<audio::streams::stream_dynamic_local_at_player*>>	streams_at_player;
	static std::map<uint16_t, std::set<audio::streams::stream_dynamic_local_at_object*>>	streams_at_object;
<<<<<<< HEAD
>>>>>>> parent of 40cfcff (upgrade to 3.0)
=======
>>>>>>> parent of 40cfcff (upgrade to 3.0)

	// Сетевой обработчик
	class net_handler : public net::handler_interface {
	public:

<<<<<<< HEAD
<<<<<<< HEAD
#include <atomic>
#include <ctime>
#include <vector>
#include <map>
#include <set>

#include <util/timer.h>
#include <util/logger.h>

#ifndef _WIN32
#define __forceinline __attribute__((always_inline))
#endif

namespace SV
{
    uint32_t bitrate { SV::kDefaultBitrate };
    std::map<uint32_t, Stream*> streamTable;
    std::set<DynamicStream*> dlstreamList;
    std::vector<WorkerPtr> workers;

    class PawnHandler : public PawnInterface {
    public:

        void SvInit(const uint32_t bitrate) override
        {
            SV::bitrate = bitrate;
        }

        uint8_t SvGetVersion(const uint16_t playerId) override
        {
            uint8_t playerPluginVersion { NULL };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) playerPluginVersion = pPlayerInfo->pluginVersion;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerPluginVersion;
        }

        // -------------------------------------------------------------------------------------

        bool SvHasMicro(const uint16_t playerId) override
        {
            bool playerHasMicroStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) playerHasMicroStatus = pPlayerInfo->microStatus;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerHasMicroStatus;
        }

        bool SvStartRecord(const uint16_t playerId) override
        {
            bool prevRecordStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevRecordStatus = pPlayerInfo->recordStatus.exchange(true);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (prevRecordStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::startRecord, NULL);

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        bool SvStopRecord(const uint16_t playerId) override
        {
            bool prevRecordStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevRecordStatus = pPlayerInfo->recordStatus.exchange(false);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (!prevRecordStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::stopRecord, NULL);

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        bool SvAddKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool addKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) addKeyStatus = pPlayerInfo->keys.insert(keyId).second;
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (!addKeyStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::addKey, sizeof(SV::AddKeyPacket));
            PackGetStruct(controlPacket, SV::AddKeyPacket)->keyId = keyId;

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        bool SvHasKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool hasKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) hasKeyStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return hasKeyStatus;
        }

        bool SvRemoveKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool removeKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) removeKeyStatus = pPlayerInfo->keys.erase(keyId);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (!removeKeyStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::removeKey, sizeof(SV::RemoveKeyPacket));
            PackGetStruct(controlPacket, SV::RemoveKeyPacket)->keyId = keyId;

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        void SvRemoveAllKeys(const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->keys.clear();
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (pPlayerInfo == nullptr) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::removeAllKeys, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        bool SvMutePlayerStatus(const uint16_t playerId) override
        {
            bool mutePlayerStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) mutePlayerStatus = pPlayerInfo->muteStatus.load();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return mutePlayerStatus;
        }

        void SvMutePlayerEnable(const uint16_t playerId) override
        {
            bool prevMutePlayerStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(true);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (prevMutePlayerStatus) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::muteEnable, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        void SvMutePlayerDisable(const uint16_t playerId) override
        {
            bool prevMutePlayerStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(false);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (!prevMutePlayerStatus) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::muteDisable, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateGStream(const uint32_t color, const std::vector<char>& name) override
        {
            const auto stream = new (std::nothrow) GlobalStream(color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateSLStreamAtPoint(
            const float distance,
            const float posx,
            const float posy,
            const float posz,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            const auto stream = new (std::nothrow) StaticLocalStreamAtPoint(distance, CVector { posx, posy, posz }, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtVehicle(
            const float distance,
            const uint16_t vehicleId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pVehiclePool->pVehicle[vehicleId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtVehicle(distance, vehicleId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtPlayer(
            const float distance,
            const uint16_t playerId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pPlayerPool->pPlayer[playerId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtPlayer(distance, playerId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtObject(
            const float distance,
            const uint16_t objectId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pObjectPool->pObjects[objectId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtObject(distance, objectId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------
=======
		// Обработчик входящих пакетов
		bool packet_handler(uint8_t packet_id, Packet *packet) {

=======
		// Обработчик входящих пакетов
		bool packet_handler(uint8_t packet_id, Packet *packet) {

>>>>>>> parent of 40cfcff (upgrade to 3.0)
			bool result = true;

			sv_packet::header *p_header = reinterpret_cast<sv_packet::header*>(packet->data);
			switch (packet_id) {
			case e_packet_id::sound_request: {
				sv_packet::sound_request *st_data = p_header->get<sv_packet::sound_request>();
				if (const char *url = audio::sounds::store::url(st_data->index)) {
					bitstream bs(e_packet_id::sound_register);
					bs.Write(st_data->index);
					bs.Write(url, strlen(url) + 1);
					net::send(packet->playerIndex, &bs);
				}
			} break;
			case e_packet_id::voice: {
				for (auto i = pawn::script::get_scripts_opv()->begin(); i != pawn::script::get_scripts_opv()->end(); i++)
					if (!(*i).onplayervoice(packet->playerIndex, packet, ((sv_packet::header*)(packet->data))->get<sv_packet::voice>()->volume))
						result = false;	// Не удалять пакет если хотя бы один паблик этого попросит
			} break;
			}

			return result;

		}

		// Обработчик подключения игрока
		void rpc_handler_connect(uint16_t sender_id, sv_packet::connect *header) {

			// Проверяем наличие плагина
			if (header->signature == SV_NET_CONNECT_SIGNATURE) {
				core::players[sender_id].sv_version = header->version;
				core::players[sender_id].has_micro = header->has_micro;
				if (debug_mode) LogDebug("rpc_handler_connect : sender_id(%hu) version(%hhu) has_micro(%hhu)", sender_id, header->version, header->has_micro);
				net::send(sender_id, e_packet_id::init, &settings);
			} else {
				core::players[sender_id].sv_version = NULL;
				core::players[sender_id].has_micro = false;
			}

		}

		// Обработчик добавления автомобиля в стрим игрока
		void rpc_handler_vehicle_add(uint16_t sender_id, uint16_t vehicle_id) {

			if (debug_mode) LogDebug("rpc_handler_vehicle_add : sender_id(%hu) vehicle_id(%hu)", sender_id, vehicle_id);

			const auto iter = streams_at_vehicle.find(vehicle_id);
			if (iter != streams_at_vehicle.end())
				for (auto i : (*iter).second)
					i->_player_attach(sender_id);

		}

		// Обработчик удаления автомобиля из стрима игрока
		void rpc_handler_vehicle_remove(uint16_t sender_id, uint16_t vehicle_id) {

			if (debug_mode) LogDebug("rpc_handler_vehicle_remove : sender_id(%hu) vehicle_id(%hu)", sender_id, vehicle_id);

			const auto iter = streams_at_vehicle.find(vehicle_id);
			if (iter != streams_at_vehicle.end())
				for (auto i : (*iter).second)
					i->_player_detach(sender_id);

		}

		// Обработчик добавления игрока в стрим другого игрока
		void rpc_handler_player_add(uint16_t sender_id, uint16_t player_id) {

			if (debug_mode) LogDebug("rpc_handler_player_add : sender_id(%hu) player_id(%hu)", sender_id, player_id);

			const auto iter = streams_at_player.find(player_id);
			if (iter != streams_at_player.end())
				for (auto i : (*iter).second)
					i->_player_attach(sender_id);

		}

		// Обработчик удаления игрока из стрима другого игрока
		void rpc_handler_player_remove(uint16_t sender_id, uint16_t player_id) {

			if (debug_mode) LogDebug("rpc_handler_player_remove : sender_id(%hu) player_id(%hu)", sender_id, player_id);

			const auto iter = streams_at_player.find(player_id);
			if (iter != streams_at_player.end())
				for (auto i : (*iter).second)
					i->_player_detach(sender_id);

		}

		// Обработчик добавления объекта в стрим игрока
		void rpc_handler_object_add(uint16_t sender_id, uint16_t object_id) {

			if (debug_mode) LogDebug("rpc_handler_object_add : sender_id(%hu) object_id(%hu)", sender_id, object_id);

			const auto iter = streams_at_object.find(object_id);
			if (iter != streams_at_object.end())
				for (auto i : (*iter).second)
					i->_player_attach(sender_id);

		}

		// Обработчик удаления объекта из стрима игрока
		void rpc_handler_object_remove(uint16_t sender_id, uint16_t object_id) {

			if (debug_mode) LogDebug("rpc_handler_object_remove : sender_id(%hu) object_id(%hu)", sender_id, object_id);

			const auto iter = streams_at_object.find(object_id);
			if (iter != streams_at_object.end())
				for (auto i : (*iter).second)
					i->_player_detach(sender_id);

		}

	};

	// Pawn-обработчик
	class pawn_handler : public pawn::handler_interface {
	public:



		// Инициализировать настройки плагина
		bool init(
			uint32_t bitrate,
			uint16_t frequency,
			uint8_t voice_rate,
			float factor_distance,
			float factor_rolloff,
			float factor_doppler
		) {

			if (frequency != 8000 && frequency != 12000 && frequency != 16000 && frequency != 24000 && frequency != 48000) return false;
			if (voice_rate != 40 && voice_rate != 60 && voice_rate != 80 && voice_rate != 100 && voice_rate != 120) return false;
			if (factor_rolloff < 0.f || factor_rolloff > 10.f) return false;
			if (factor_doppler < 0.f || factor_doppler > 10.f) return false;
			
			settings.bitrate = bitrate;
			settings.frequency = frequency;
			settings.voice_rate = voice_rate;
			settings.factor_distance = factor_distance;
			settings.factor_rolloff = factor_rolloff;
			settings.factor_doppler = factor_doppler;

			return true;

		}

		// Получить версию клиентского плагина
		uint8_t get_version(
			uint16_t player_id
		) {
			if (samp::player_is_connect(player_id))
				return players[player_id].sv_version;
			else return players[player_id].sv_version = NULL;
		}

		// Установить клавишу активации
		void set_key(
			uint16_t player_id,
			uint8_t key_id
		) {
			if (samp::player_is_connect(player_id)) {
				sv_packet::set_key st_params = { key_id };
				net::send(player_id, e_packet_id::set_key, &st_params);
			}
		}

		// Отправить пакет в ресивер
		void send_packet(
			Packet *packet,
			audio::receiver *target
		) {
			if (audio::object::is_object(target)) target->push(packet);
		}



		// Проверить наличие микрофона
		bool has_micro(
			uint16_t player_id
		) {
			return players[player_id].has_micro = players[player_id].has_micro && samp::player_is_connect(player_id);
		}

		// Включить микрофон
		void record_start(
			uint16_t player_id
		) {
			if (samp::player_is_connect(player_id)) net::send(player_id, e_packet_id::record_start);
		}

		// Отключить микрофон
		void record_stop(
			uint16_t player_id
		) {
			if (samp::player_is_connect(player_id)) net::send(player_id, e_packet_id::record_stop);
		}

		// Настроить чувствительность микрофона
		void record_volume(
			uint16_t player_id,
			float volume
		) {
			if (samp::player_is_connect(player_id)) {
				sv_packet::record_volume st_params = { volume };
				net::send(player_id, e_packet_id::record_volume, &st_params);
			}
		}



		// Создать эффект "chorus"
		audio::effects::chorus* effect_create_chorus(
			int priority,
			float wetdrymix,
			float depth,
			float feedback,
			float frequency,
			uint32_t waveform,
			float delay,
			uint32_t phase
		) {
			return new audio::effects::chorus(
				priority,
				wetdrymix,
				depth,
				feedback,
				frequency,
				waveform,
				delay,
				phase
			);
		}

		// Создать эффект "compressor"
		audio::effects::compressor* effect_create_compressor(
			int priority,
			float gain,
			float attack,
			float release,
			float threshold,
			float ratio,
			float predelay
		) {
			return new audio::effects::compressor(
				priority,
				gain,
				attack,
				release,
				threshold,
				ratio,
				predelay
			);
		}

		// Создать эффект "distortion"
		audio::effects::distortion* effect_create_distortion(
			int priority,
			float gain,
			float edge,
			float posteqcenterfrequency,
			float posteqbandwidth,
			float prelowpasscutoff
		) {
			return new audio::effects::distortion(
				priority,
				gain,
				edge,
				posteqcenterfrequency,
				posteqbandwidth,
				prelowpasscutoff
			);
		}

		// Создать эффект "echo"
		audio::effects::echo* effect_create_echo(
			int priority,
			float wetdrymix,
			float feedback,
			float leftdelay,
			float rightdelay,
			bool pandelay
		) {
			return new audio::effects::echo(
				priority,
				wetdrymix,
				feedback,
				leftdelay,
				rightdelay,
				pandelay
			);
		}

		// Создать эффект "flanger"
		audio::effects::flanger* effect_create_flanger(
			int priority,
			float wetdrymix,
			float depth,
			float feedback,
			float frequency,
			uint32_t waveform,
			float delay,
			uint32_t phase
		) {
			return new audio::effects::flanger(
				priority,
				wetdrymix,
				depth,
				feedback,
				frequency,
				waveform,
				delay,
				phase
			);
		}

		// Создать эффект "gargle"
		audio::effects::gargle* effect_create_gargle(
			int priority,
			uint32_t ratehz,
			uint32_t waveshape
		) {
			return new audio::effects::gargle(
				priority,
				ratehz,
				waveshape
			);
		}

		// Создать эффект "i3dl2reverb"
		audio::effects::i3dl2reverb* effect_create_i3dl2reverb(
			int priority,
			int room,
			int roomhf,
			float roomrollofffactor,
			float decaytime,
			float decayhfratio,
			int reflections,
			float reflectionsdelay,
			int reverb,
			float reverbdelay,
			float diffusion,
			float density,
			float hfreference
		) {
			return new audio::effects::i3dl2reverb(
				priority,
				room,
				roomhf,
				roomrollofffactor,
				decaytime,
				decayhfratio,
				reflections,
				reflectionsdelay,
				reverb,
				reverbdelay,
				diffusion,
				density,
				hfreference
			);
		}

		// Создать эффект "parameq"
		audio::effects::parameq* effect_create_parameq(
			int priority,
			float center,
			float bandwidth,
			float gain
		) {
			return new audio::effects::parameq(
				priority,
				center,
				bandwidth,
				gain
			);
		}

		// Создать эффект "reverb"
		audio::effects::reverb* effect_create_reverb(
			int priority,
			float ingain,
			float reverbmix,
			float reverbtime,
			float highfreqrtratio
		) {
			return new audio::effects::reverb(
				priority,
				ingain,
				reverbmix,
				reverbtime,
				highfreqrtratio
			);
		}

		// Удалить эффект
		void effect_delete(
			audio::effects::effect* effect
		) {
			if (audio::object::is_object(effect)) delete effect;
		}



		// Установить параметр потокового класса
		void stream_set_parameter(
			audio::streams::streamable* target,
			uint8_t param_id,
			float value
		) {
			if (audio::object::is_object(target)) target->set_parameter(param_id, value);
		}

		// Задать скольжение параметра потокового класса
		void stream_slide_parameter(
			audio::streams::streamable* target,
			uint8_t param_id,
			float start_value,
			float end_value,
			uint32_t time
		) {
			if (audio::object::is_object(target)) target->slide_parameter(param_id, start_value, end_value, time);
		}

		// Привязать эффект к потоковому классу
		void stream_effect_attach(
			audio::streams::streamable *target,
			audio::effects::effectable *effect
		) {
			if (audio::object::is_object(target) && audio::object::is_object(effect)) target->effect_attach(effect);
		}

		// Отвязать эффект от потокового класса
		void stream_effect_detach(
			audio::streams::streamable *target,
			audio::effects::effectable *effect
		) {
			if (audio::object::is_object(target) && audio::object::is_object(effect)) target->effect_detach(effect);
		}

		// Начать воспроизведение в потоковый класс
		bool stream_play_sound_internal(
			audio::streams::streamable *target,
			uint32_t index,
			bool loop
		) {
			return (audio::object::is_object(target) && target->play_sound(index, loop));
		}

		// Начать воспроизведение в потоковый класс
		bool stream_play_sound_external(
			audio::streams::streamable *target,
			const char *url,
			bool loop
		) {
			return (audio::object::is_object(target) && target->play_sound(url, loop));
		}

		// Остановить воспроизведение в потоковый класс
		void stream_stop_sound(
			audio::streams::streamable *target
		) {
			if (audio::object::is_object(target)) target->stop_sound();
		}

		// Привязать игрока к потоковому классу
		void stream_player_attach(
			audio::streams::streamable* target,
			uint16_t player_id
		) {
			if (audio::object::is_object(target)) target->player_attach(player_id);
		}

		// Отвязать игрока от потокового класса
		void stream_player_detach(
			audio::streams::streamable* target,
			uint16_t player_id
		) {
			if (audio::object::is_object(target)) target->player_detach(player_id);
		}

		// Удалить поток
		void stream_delete(
			audio::streams::stream* stream
		) {

			if (audio::object::is_object(stream)) {
				
				// Удаление из списков автоматической привязки
				streams_at_point.erase((audio::streams::stream_dynamic_local_at_point*)(stream));
				for (auto i = streams_at_vehicle.begin(); i != streams_at_vehicle.end(); i++)
					(*i).second.erase((audio::streams::stream_dynamic_local_at_vehicle*)(stream));
				for (auto i = streams_at_player.begin(); i != streams_at_player.end(); i++)
					(*i).second.erase((audio::streams::stream_dynamic_local_at_player*)(stream));
				for (auto i = streams_at_object.begin(); i != streams_at_object.end(); i++)
					(*i).second.erase((audio::streams::stream_dynamic_local_at_object*)(stream));

				delete stream;

			}

		}



		// Создать глобальный поток
		audio::streams::stream_static_global* sgstream_create() {
			return new audio::streams::stream_static_global;
		}

		// Обновить параметры локального потока
		void slstream_update(
			audio::streams::stream_static_local_at_point *slstream,
			float pos_x, float pos_y, float pos_z,
			float ornt_x, float ornt_y, float ornt_z,
			float vel_x, float vel_y, float vel_z
		) {
			if (audio::object::is_object(slstream))
				slstream->update(
					pos_x, pos_y, pos_z,
					ornt_x, ornt_y, ornt_z,
					vel_x, vel_y, vel_z
				);
		}

		// Создать статический локальный поток с привязкой к точке
		audio::streams::stream_static_local_at_point* slstream_create_at_point(
			float pos_x, float pos_y, float pos_z,
			float ornt_x, float ornt_y, float ornt_z,
			float vel_x, float vel_y, float vel_z
		) {
			return new audio::streams::stream_static_local_at_point(
				pos_x, pos_y, pos_z,
				ornt_x, ornt_y, ornt_z,
				vel_x, vel_y, vel_z
			);
		}

		// Создать статический локальный поток с привязкой к автомобилю
		audio::streams::stream_static_local_at_vehicle* slstream_create_at_vehicle(
			uint16_t vehicle_id
		) {
			return new audio::streams::stream_static_local_at_vehicle(vehicle_id);
		}

		// Создать статический локальный поток с привязкой к игроку
		audio::streams::stream_static_local_at_player* slstream_create_at_player(
			uint16_t player_id
		) {
			return new audio::streams::stream_static_local_at_player(player_id);
		}

		// Создать статический локальный поток с привязкой к объекту
		audio::streams::stream_static_local_at_object* slstream_create_at_object(
			uint16_t object_id
		) {
			return new audio::streams::stream_static_local_at_object(object_id);
		}



		// Создать динамический локальный поток с привязкой к точке
		audio::streams::stream_dynamic_local_at_point* dlstream_create_at_point(
			float pos_x, float pos_y, float pos_z,
			float ornt_x, float ornt_y, float ornt_z,
			float vel_x, float vel_y, float vel_z
		) {

			const auto result = new audio::streams::stream_dynamic_local_at_point(
				pos_x, pos_y, pos_z,
				ornt_x, ornt_y, ornt_z,
				vel_x, vel_y, vel_z
			);

			if (result) streams_at_point.insert(result);
			return result;

		}

		// Создать динамический локальный поток с привязкой к автомобилю
		audio::streams::stream_dynamic_local_at_vehicle* dlstream_create_at_vehicle(
			uint16_t vehicle_id
		) {
			const auto result = new audio::streams::stream_dynamic_local_at_vehicle(vehicle_id);
			if (result) streams_at_vehicle[vehicle_id].insert(result);
			return result;
		}

		// Создать динамический локальный поток с привязкой к игроку
		audio::streams::stream_dynamic_local_at_player* dlstream_create_at_player(
			uint16_t player_id
		) {
			const auto result = new audio::streams::stream_dynamic_local_at_player(player_id);
			if (result) streams_at_player[player_id].insert(result);
			return result;
		}

		// Создать динамический локальный поток с привязкой к объекту
		audio::streams::stream_dynamic_local_at_object* dlstream_create_at_object(
			uint16_t object_id
		) {
			const auto result = new audio::streams::stream_dynamic_local_at_object(object_id);
			if (result) streams_at_object[object_id].insert(result);
			return result;
		}



		// Добавить элемент в группу
		void group_add(
			audio::group *group,
			void *element
		) {
			if (audio::object::is_object(group)) group->add(element);
		}

		// Удалить элемент из группы
		void group_remove(
			audio::group *group,
			void *element
		) {
			if (audio::object::is_object(group)) group->remove(element);
		}

		// Удалить группу
		void group_delete(
			audio::group *group
		) {
			if (audio::object::is_object(group)) delete group;
		}



		// Создать группу потоков
		audio::streams::stream_group* sgroup_create() {
			return new audio::streams::stream_group;
		}



		// Создать группу эффектов
		audio::effects::effect_group* egroup_create() {
			return new audio::effects::effect_group;
		}



		// Создать буфер
		audio::buffer* buffer_create() {
			return new audio::buffer;
		}

		// Очистить буфер
		void buffer_clear(
			audio::buffer *buffer
		) {
			if (audio::object::is_object(buffer)) buffer->clear();
		}

		// Сохранить буфер
		bool buffer_save(
			audio::buffer *buffer,
			const char *filename
		) {
			if (audio::object::is_object(buffer)) return buffer->save(filename);
			else return false;
		}

		// Загрузить буфер
		bool buffer_load(
			audio::buffer *buffer,
			const char *filename
		) {
			if (audio::object::is_object(buffer)) return buffer->load(filename);
			else return false;
		}

		// Привязать буфер
		void buffer_attach(
			audio::buffer *buffer,
			audio::streams::streamable *target
		) {
			if (audio::object::is_object(buffer) && audio::object::is_object(target)) buffer->attach(target);
		}

		// Отвязать буфер
		void buffer_detach(
			audio::buffer *buffer,
			audio::streams::streamable *target
		) {
			if (audio::object::is_object(buffer) && audio::object::is_object(target)) buffer->detach(target);
		}

		// Отвязать буфер от всех потоковых классов
		void buffer_detach_all(
			audio::buffer *buffer
		) {
			if (audio::object::is_object(buffer)) buffer->detach_all();
		}

		// Начать трансляцию
		void buffer_play(
			audio::buffer *buffer,
			bool loop
		) {
			if (audio::object::is_object(buffer)) buffer->play(loop);
		}

		// Остановить трансляцию
		void buffer_stop(
			audio::buffer *buffer
		) {
			if (audio::object::is_object(buffer)) buffer->stop();
		}

		// Удалить буфер
		void buffer_delete(
			audio::buffer *buffer
		) {
			if (audio::object::is_object(buffer)) delete buffer;
		}



		// Зарегистрировать звук
		uint32_t sound_register(
			const char *url
		) {
			return audio::sounds::store::sound(url);
		}



	};
<<<<<<< HEAD
>>>>>>> parent of 40cfcff (upgrade to 3.0)

        Stream* SvCreateDLStreamAtPoint(
            const float distance,
            const uint32_t maxPlayers,
            const float posx,
            const float posy,
            const float posz,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            const auto stream = new (std::nothrow) DynamicLocalStreamAtPoint(distance, maxPlayers, CVector { posx, posy, posz }, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtVehicle(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t vehicleId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pVehiclePool->pVehicle[vehicleId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtVehicle(distance, maxPlayers, vehicleId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtPlayer(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t playerId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pPlayerPool->pPlayer[playerId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtPlayer(distance, maxPlayers, playerId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtObject(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t objectId,
            const uint32_t color,
            const std::vector<char>& name
        ) override
        {
            if (pNetGame->pObjectPool->pObjects[objectId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtObject(distance, maxPlayers, objectId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------

        void SvUpdatePositionForLPStream(PointStream* const lpStream, const float posx, const float posy, const float posz) override
        {
            lpStream->UpdatePosition(CVector(posx, posy, posz));
        }

        void SvUpdateDistanceForLStream(LocalStream* const lStream, const float distance) override
        {
            lStream->UpdateDistance(distance);
        }

        // -------------------------------------------------------------------------------------

        bool SvAttachListenerToStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.insert(stream);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return stream->AttachListener(playerId);
        }

        bool SvHasListenerInStream(Stream* const stream, const uint16_t playerId) override
        {
            return stream->HasListener(playerId);
        }

        bool SvDetachListenerFromStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return stream->DetachListener(playerId);
        }

        void SvDetachAllListenersFromStream(Stream* const stream) override
        {
            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        bool SvAttachSpeakerToStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.insert(stream);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            return stream->AttachSpeaker(playerId);
        }

        bool SvHasSpeakerInStream(Stream* const stream, const uint16_t playerId) override
        {
            return stream->HasSpeaker(playerId);
        }

        bool SvDetachSpeakerFromStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            return stream->DetachSpeaker(playerId);
        }

        void SvDetachAllSpeakersFromStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        void SvStreamParameterSet(Stream* const stream, const uint8_t parameter, const float value) override
        {
            stream->SetParameter(parameter, value);
        }

        void SvStreamParameterReset(Stream* const stream, const uint8_t parameter) override
        {
            stream->ResetParameter(parameter);
        }

        bool SvStreamParameterHas(Stream* const stream, const uint8_t parameter) override
        {
            return stream->HasParameter(parameter);
        }

        float SvStreamParameterGet(Stream* const stream, const uint8_t parameter) override
        {
            return stream->GetParameter(parameter);
        }

        void SvStreamParameterSlideFromTo(Stream* const stream, const uint8_t parameter, const float startvalue, const float endvalue, const uint32_t time) override
        {
            stream->SlideParameterFromTo(parameter, startvalue, endvalue, time);
        }

        void SvStreamParameterSlideTo(Stream* const stream, const uint8_t parameter, const float endvalue, const uint32_t time) override
        {
            stream->SlideParameterTo(parameter, endvalue, time);
        }

        void SvStreamParameterSlide(Stream* const stream, const uint8_t parameter, const float deltavalue, const uint32_t time) override
        {
            stream->SlideParameter(parameter, deltavalue, time);
        }

        // -------------------------------------------------------------------------------------

        void SvDeleteStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }

            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }

            SV::streamTable.erase(reinterpret_cast<uint32_t>(stream));
            if (const auto dlStream = dynamic_cast<DynamicStream*>(stream))
                SV::dlstreamList.erase(dlStream);

            delete stream;
        }

        // -------------------------------------------------------------------------------------

        Effect* SvEffectCreateChorus(const int priority, const float wetdrymix, const float depth, const float feedback, const float frequency, const uint32_t waveform, const float delay, const uint32_t phase) override
        {
            return new (std::nothrow) Effect(SV::EffectType::chorus, priority, ChorusParameters { wetdrymix, depth, feedback, frequency, waveform, delay, phase });
        }

        Effect* SvEffectCreateCompressor(const int priority, const float gain, const float attack, const float release, const float threshold, const float ratio, const float predelay) override
        {
            return new (std::nothrow) Effect(SV::EffectType::compressor, priority, CompressorParameters { gain, attack, release, threshold, ratio, predelay });
        }

        Effect* SvEffectCreateDistortion(const int priority, const float gain, const float edge, const float posteqcenterfrequency, const float posteqbandwidth, const float prelowpasscutoff) override
        {
            return new (std::nothrow) Effect(SV::EffectType::distortion, priority, DistortionParameters { gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff });
        }

        Effect* SvEffectCreateEcho(const int priority, const float wetdrymix, const float feedback, const float leftdelay, const float rightdelay, const bool pandelay) override
        {
            return new (std::nothrow) Effect(SV::EffectType::echo, priority, EchoParameters { wetdrymix, feedback, leftdelay, rightdelay, pandelay });
        }

        Effect* SvEffectCreateFlanger(const int priority, const float wetdrymix, const float depth, const float feedback, const float frequency, const uint32_t waveform, const float delay, const uint32_t phase) override
        {
            return new (std::nothrow) Effect(SV::EffectType::flanger, priority, FlangerParameters { wetdrymix, depth, feedback, frequency, waveform, delay, phase });
        }

        Effect* SvEffectCreateGargle(const int priority, const uint32_t ratehz, const uint32_t waveshape) override
        {
            return new (std::nothrow) Effect(SV::EffectType::gargle, priority, GargleParameters { ratehz, waveshape });
        }

        Effect* SvEffectCreateI3dl2reverb(const int priority, const int room, const int roomhf, const float roomrollofffactor, const float decaytime, const float decayhfratio, const int reflections, const float reflectionsdelay, const int reverb, const float reverbdelay, const float diffusion, const float density, const float hfreference) override
        {
            return new (std::nothrow) Effect(SV::EffectType::i3dl2reverb, priority, I3dl2reverbParameters { room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference });
        }

        Effect* SvEffectCreateParameq(const int priority, const float center, const float bandwidth, const float gain) override
        {
            return new (std::nothrow) Effect(SV::EffectType::parameq, priority, ParameqParameters { center, bandwidth, gain });
        }

        Effect* SvEffectCreateReverb(const int priority, const float ingain, const float reverbmix, const float reverbtime, const float highfreqrtratio) override
        {
            return new (std::nothrow) Effect(SV::EffectType::reverb, priority, ReverbParameters { ingain, reverbmix, reverbtime, highfreqrtratio });
        }

        void SvEffectAttachStream(Effect* const effect, Stream* const stream) override
        {
            effect->AttachStream(stream);
        }

        void SvEffectDetachStream(Effect* const effect, Stream* const stream) override
        {
            effect->DetachStream(stream);
        }

        void SvEffectDelete(Effect* const effect) override
        {
            delete effect;
        }

    };

    void ConnectHandler(const uint16_t playerId, const SV::ConnectPacket& connectStruct) noexcept
    {
        PlayerStore::AddPlayerToStore(playerId, connectStruct.version, connectStruct.micro);
    }

    void PlayerInitHandler(const uint16_t playerId, SV::PluginInitPacket& initStruct) noexcept
    {
        initStruct.bitrate = SV::bitrate;

        const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
        if (pPlayerInfo != nullptr) initStruct.mute = pPlayerInfo->muteStatus.load(std::memory_order_relaxed);
        PlayerStore::ReleasePlayerWithSharedAccess(playerId);
    }

    void DisconnectHandler(const uint16_t playerId) noexcept
    {
        PlayerStore::RemovePlayerFromStore(playerId);
    }

    static __forceinline void Tick() noexcept
    {
        for (const auto dlStream : SV::dlstreamList)
            dlStream->Tick();

        uint16_t senderId { SV::kNonePlayer };

        while (const auto controlPacket = Network::ReceiveControlPacket(senderId))
        {
            const auto& controlPacketRef = *controlPacket;

            switch (controlPacketRef->packet)
            {
                case SV::ControlPacketType::pressKey:
                {
                    const auto stData = PackGetStruct(&controlPacketRef, SV::PressKeyPacket);
                    if (controlPacketRef->length != sizeof(*stData)) break;

                    const auto keyId = stData->keyId;
                    bool pressKeyAllowStatus { false };

                    const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(senderId);
                    if (pPlayerInfo != nullptr) pressKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
                    PlayerStore::ReleasePlayerWithSharedAccess(senderId);

                    if (!pressKeyAllowStatus) break;

                    Pawn::OnPlayerActivationKeyPressForAll(senderId, keyId);
                } break;
                case SV::ControlPacketType::releaseKey:
                {
                    const auto stData = PackGetStruct(&controlPacketRef, SV::ReleaseKeyPacket);
                    if (controlPacketRef->length != sizeof(*stData)) break;

                    const auto keyId = stData->keyId;
                    bool releaseKeyAllowStatus { false };

                    const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(senderId);
                    if (pPlayerInfo != nullptr) releaseKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
                    PlayerStore::ReleasePlayerWithSharedAccess(senderId);

                    if (!releaseKeyAllowStatus) break;

                    Pawn::OnPlayerActivationKeyReleaseForAll(senderId, keyId);
                } break;
            }
        }

        Network::Process();
    }
}

// Инициализация плагина
static void init() {

<<<<<<< HEAD
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() noexcept
{
    Timer::Tick();
    SV::Tick();
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() noexcept
{
    static bool unloadStatus { false };
    if (unloadStatus) return;
    unloadStatus = true;

    Logger::Log(" -------------------------------------------");
    Logger::Log("           SampVoice unloading...           ");
    Logger::Log(" -------------------------------------------");

    SV::workers.clear();

    PlayerStore::ClearStore();

    Pawn::Free();
    RakNet::Free();
    Network::Free();
    Logger::Free();
}

#ifdef _WIN32
BOOL WINAPI WinExitHandler(DWORD) noexcept
{
    Unload();
    return FALSE;
}
#endif

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** const ppData) noexcept
{
#ifdef _WIN32
    SetConsoleCtrlHandler(&WinExitHandler, TRUE);
#endif

    ppPluginData = ppData;
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);

    if (!Logger::Init(SV::kLogFileName, logprintf))
    {
        logprintf("[sv:err:main:Load] : failed to init logger");
        return false;
    }

    if (!Config::Load())
    {
        Logger::Log("[sv:inf:main:Load] : failed to load 'server.cfg'");
    }

    if (!Network::Init(logprintf))
    {
        Logger::Log("[sv:err:main:Load] : failed to init network");
        Logger::Free();
        return false;
    }

    Network::AddConnectCallback(SV::ConnectHandler);
    Network::AddPlayerInitCallback(SV::PlayerInitHandler);
    Network::AddDisconnectCallback(SV::DisconnectHandler);

    if (!Pawn::Init(std::make_unique<SV::PawnHandler>()))
    {
        Logger::Log("[sv:err:main:Load] : failed to init pawn");
        Network::Free();
        Logger::Free();
        return false;
    }

    {
        auto nprocs = std::thread::hardware_concurrency();

        if (!nprocs || nprocs > SV::kVoiceThreadsCount)
            nprocs = SV::kVoiceThreadsCount;

        Logger::Log("[sv:dbg:main:Load] : creating %u work threads...", nprocs);

        SV::workers.reserve(nprocs); for (auto i { nprocs }; i > 0; --i)
            SV::workers.emplace_back(MakeWorker());
    }

    Logger::Log(" -------------------------------------------    ");
    Logger::Log("   ___                __   __    _              ");
    Logger::Log("  / __| __ _ _ __  _ _\\ \\ / /__ (_) __ ___    ");
    Logger::Log("  \\__ \\/ _` | '  \\| '_ \\   / _ \\| |/ _/ -_)");
    Logger::Log("  |___/\\__,_|_|_|_| .__/\\_/\\___/|_|\\__\\___|");
    Logger::Log("                  |_|                           ");
    Logger::Log(" -------------------------------------------    ");
    Logger::Log("           SampVoice by MOR loaded              ");
    Logger::Log(" -------------------------------------------    ");
=======
	if (!(pNetGame = reinterpret_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])()) ||
		!(pConsole = reinterpret_cast<void*(*)()>(ppPluginData[PLUGIN_DATA_CONSOLE])()) ||
		!(pRakServer = reinterpret_cast<RakServerInterface*(*)()>(ppPluginData[PLUGIN_DATA_RAKSERVER])())
	) {
		LogError("main", "could not initialize main structures");
		return;
	}
=======
>>>>>>> parent of 40cfcff (upgrade to 3.0)

	CAddress::Initialize(logprintf);
	console::init(pConsole);

	if (!BASS_Init(0, SV_SETTINGS_DEFAULT_FREQUENCY, 0, nullptr, nullptr))
		LogError("main", "could not initialize bass library (code:%d)", BASS_ErrorGetCode());

	pawn::init(new core::pawn_handler);

}

<<<<<<< HEAD
=======
// Инициализация плагина
static void init() {

	if (!(pNetGame = reinterpret_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])()) ||
		!(pConsole = reinterpret_cast<void*(*)()>(ppPluginData[PLUGIN_DATA_CONSOLE])()) ||
		!(pRakServer = reinterpret_cast<RakServerInterface*(*)()>(ppPluginData[PLUGIN_DATA_RAKSERVER])())
	) {
		LogError("main", "could not initialize main structures");
		return;
	}

	CAddress::Initialize(logprintf);
	console::init(pConsole);

	if (!BASS_Init(0, SV_SETTINGS_DEFAULT_FREQUENCY, 0, nullptr, nullptr))
		LogError("main", "could not initialize bass library (code:%d)", BASS_ErrorGetCode());

	pawn::init(new core::pawn_handler);

}

>>>>>>> parent of 40cfcff (upgrade to 3.0)
// Главный цикл обработки
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {

	if (pNetGame->pPlayerPool->dwConnectedPlayers) {

		// Проверка привязанных статических потоков
		for (auto i = core::streams_at_point.begin(); i != core::streams_at_point.end(); i++)
			for (uint16_t pid = 0; pid <= pNetGame->pPlayerPool->dwPlayerPoolSize; pid++)
				if (pNetGame->pPlayerPool->bIsPlayerConnected[pid]) {
					if ((*i)->is_in_stream(pid)) (*i)->_player_attach(pid);
					else (*i)->_player_detach(pid);
				}

	}

}

// Загрузка плагина
PLUGIN_EXPORT bool PLUGIN_CALL Load(
	void **ppData
) {
	
	ppPluginData = ppData;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	if (!logger::init(logprintf)) {
		logprintf("[sampvoice] : [error] : [main] : [%s] : could not logger initialize", __func__);
		return false;
	}

	/*
	DWORD logprintf_addr = reinterpret_cast<DWORD>(ppData[PLUGIN_DATA_LOGPRINTF]);
	if (logprintf_addr != CAddress::FUNC_Logprintf_037 &&
		logprintf_addr != CAddress::FUNC_Logprintf_037_R2_1
	) {
		LogError("main", "plugin supports only version 0.3.7");
		return false;
	}
	*/

	if (!net::init(
		ppData[PLUGIN_DATA_LOGPRINTF],
		new core::net_handler
	)) {
		LogError("main", "could not initialize net module");
		return false;
	}

	logprintf("SAMPVOICE plugin " SV_CURRENT_VERSION_TEXT " by MOR loaded");
	return true;
>>>>>>> parent of 40cfcff (upgrade to 3.0)

    return true;
}

<<<<<<< HEAD
<<<<<<< HEAD
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* const amx) noexcept
{
    if (pNetGame == nullptr && (pNetGame = reinterpret_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])()) != nullptr)
        Logger::Log("[sv:dbg:main:AmxLoad] : net game pointer (value:%p) received", pNetGame);

    if (!Network::Bind()) Logger::Log("[sv:dbg:main:AmxLoad] : failed to bind voice server");

    Pawn::RegisterScript(amx);
=======
// Выгрузка плагина
PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	pawn::free();
	net::free();
	BASS_Free();
}

// Загрузка мода
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {

	static bool plugin_init = true;
	if (plugin_init) {
		plugin_init = false;
		init();
	}

=======
// Выгрузка плагина
PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	pawn::free();
	net::free();
	BASS_Free();
}

// Загрузка мода
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {

	static bool plugin_init = true;
	if (plugin_init) {
		plugin_init = false;
		init();
	}

>>>>>>> parent of 40cfcff (upgrade to 3.0)
	pawn::script::reg(amx);
	return AMX_ERR_NONE;
>>>>>>> parent of 40cfcff (upgrade to 3.0)

    return AMX_ERR_NONE;
}

<<<<<<< HEAD
<<<<<<< HEAD
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX*) noexcept
{
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() noexcept
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
=======
=======
>>>>>>> parent of 40cfcff (upgrade to 3.0)
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	return AMX_ERR_NONE;
}
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
<<<<<<< HEAD
>>>>>>> parent of 40cfcff (upgrade to 3.0)
=======
>>>>>>> parent of 40cfcff (upgrade to 3.0)
}
