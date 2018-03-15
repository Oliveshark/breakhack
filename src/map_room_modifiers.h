#ifndef MAP_ROOM_MODIFIERS_H_
#define	MAP_ROOM_MODIFIERS_H_


typedef enum RoomModifierType_e {
	RMOD_TYPE_NONE,
	RMOD_TYPE_WINDY
} RoomModifierType;

typedef struct WindData_t {
	Vector2d direction;
} WindData;

typedef union RoomModifierDataContainer_t {
	WindData wind;
} RoomModifierDataContainer;

typedef struct RoomModifierData_t {
	RoomModifierType type;
	RoomModifierDataContainer data;
} RoomModifierData;

#endif // MAP_ROOM_MODIFIERS_H_
