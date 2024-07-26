/**
 * @file main.c
 * @author 王一赫 (wyihe5220@gmail.com)
 * @brief KMP算法的实现实验
 * @version 0.1
 * @date 2024-05-31
 * 
 * @copyright copyright (c) 2024
 *   Licensed to the Apache Software Foundation (ASF) under one
 *   or more contributor license agreements.  See the NOTICE file
 *   distributed with this work for additional information
 *   regarding copyright ownership.  The ASF licenses this file
 *   to you under the Apache License, Version 2.0 (the
 *   "License"); you may not use this file except in compliance
 *   with the License.  You may obtain a copy of the License at
 *  
 *     http://www.apache.org/licenses/LICENSE-2.0
 *  
 *   Unless required by applicable law or agreed to in writing,
 *   software distributed under the License is distributed on an
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *   KIND, either express or implied.  See the License for the
 *   specific language governing permissions and limitations
 *   under the License.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <Windows.h>

#define MAX_PLAYERS 4
#define MAX_PROPERTIES 10
#define MAX_GIFTS 5

typedef struct {
    char name[50];
    int houses;
} Property;

typedef struct {
    int id;
    char name[50];
    int position;
    int funds;
    int points;
    Property properties[MAX_PROPERTIES];
    int properties_count;
    int block;
    int robot;
    int bomb;
    char gifts[MAX_GIFTS][50];
    int gifts_count;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    int players_count;
} GameState;

_Bool load_game_state_from_json(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return 0;
    }

    fread(buffer, 1, length, file);
    fclose(file);
    buffer[length] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return 0;
    }

    cJSON *game_state_json = cJSON_GetObjectItemCaseSensitive(json, "gameState");
    if (!cJSON_IsObject(game_state_json)) {
        cJSON_Delete(json);
        return 0;
    }

    cJSON *players_json = cJSON_GetObjectItemCaseSensitive(game_state_json, "players");
    if (!cJSON_IsArray(players_json)) {
        cJSON_Delete(json);
        return 0;
    }

    game_state->players_count = 0;
    cJSON *player_json = NULL;
    cJSON_ArrayForEach(player_json, players_json) {
        if (game_state->players_count >= MAX_PLAYERS) break;

        Player *player = &game_state->players[game_state->players_count];
        player->properties_count = 0;
        player->gifts_count = 0;

        cJSON *id_json = cJSON_GetObjectItemCaseSensitive(player_json, "id");
        cJSON *name_json = cJSON_GetObjectItemCaseSensitive(player_json, "name");
        cJSON *position_json = cJSON_GetObjectItemCaseSensitive(player_json, "position");
        cJSON *funds_json = cJSON_GetObjectItemCaseSensitive(player_json, "funds");
        cJSON *points_json = cJSON_GetObjectItemCaseSensitive(player_json, "points");
        cJSON *properties_json = cJSON_GetObjectItemCaseSensitive(player_json, "properties");
        cJSON *tools_json = cJSON_GetObjectItemCaseSensitive(player_json, "tools");
        cJSON *gifts_json = cJSON_GetObjectItemCaseSensitive(player_json, "gifts");

        if (cJSON_IsNumber(id_json)) player->id = id_json->valueint;
        if (cJSON_IsString(name_json) && (name_json->valuestring != NULL)) strncpy(player->name, name_json->valuestring, sizeof(player->name));
        if (cJSON_IsNumber(position_json)) player->position = position_json->valueint;
        if (cJSON_IsNumber(funds_json)) player->funds = funds_json->valueint;
        if (cJSON_IsNumber(points_json)) player->points = points_json->valueint;

        if (cJSON_IsArray(properties_json)) {
            cJSON *property_json = NULL;
            cJSON_ArrayForEach(property_json, properties_json) {
                if (player->properties_count >= MAX_PROPERTIES) break;
                Property *property = &player->properties[player->properties_count];

                cJSON *property_name_json = cJSON_GetObjectItemCaseSensitive(property_json, "name");
                cJSON *houses_json = cJSON_GetObjectItemCaseSensitive(property_json, "houses");

                if (cJSON_IsString(property_name_json) && (property_name_json->valuestring != NULL)) strncpy(property->name, property_name_json->valuestring, sizeof(property->name));
                if (cJSON_IsNumber(houses_json)) property->houses = houses_json->valueint;

                player->properties_count++;
            }
        }

        if (cJSON_IsObject(tools_json)) {
            cJSON *block_json = cJSON_GetObjectItemCaseSensitive(tools_json, "block");
            cJSON *robot_json = cJSON_GetObjectItemCaseSensitive(tools_json, "robot");
            cJSON *bomb_json = cJSON_GetObjectItemCaseSensitive(tools_json, "bomb");

            if (cJSON_IsNumber(block_json)) player->block = block_json->valueint;
            if (cJSON_IsNumber(robot_json)) player->robot = robot_json->valueint;
            if (cJSON_IsNumber(bomb_json)) player->bomb = bomb_json->valueint;
        }

        if (cJSON_IsArray(gifts_json)) {
            cJSON *gift_json = NULL;
            cJSON_ArrayForEach(gift_json, gifts_json) {
                if (player->gifts_count >= MAX_GIFTS) break;
                if (cJSON_IsString(gift_json) && (gift_json->valuestring != NULL)) {
                    strncpy(player->gifts[player->gifts_count], gift_json->valuestring, sizeof(player->gifts[player->gifts_count]));
                    player->gifts_count++;
                }
            }
        }

        game_state->players_count++;
    }

    cJSON_Delete(json);
    return 1;
}

int main() {
    // utf-8编码
    SetConsoleOutputCP(65001);
    GameState game_state;
    if (load_game_state_from_json(&game_state, "./.res/users.json")) {
        printf("Game state loaded successfully.\n");

        for (int i = 0; i < game_state.players_count; i++) {
            Player *player = &game_state.players[i];
            printf("Player %d:\n", player->id);
            printf("  Name: %s\n", player->name);
            printf("  Position: %d\n", player->position);
            printf("  Funds: %d\n", player->funds);
            printf("  Points: %d\n", player->points);
            printf("  Properties:\n");
            for (int j = 0; j < player->properties_count; j++) {
                printf("    %s: %d houses\n", player->properties[j].name, player->properties[j].houses);
            }
            printf("  Tools:\n");
            printf("    Block: %d\n", player->block);
            printf("    Robot: %d\n", player->robot);
            printf("    Bomb: %d\n", player->bomb);
            printf("  Gifts:\n");
            for (int k = 0; k < player->gifts_count; k++) {
                printf("    %s\n", player->gifts[k]);
            }
        }
    } else {
        printf("Failed to load game state.\n");
    }

    return 0;
}
