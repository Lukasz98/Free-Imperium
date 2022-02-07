#pragma once


enum class ClickEventType {
    NONE = 0,
    QUIT_GAME = 1,
    START_ROOM = 2,
    MISS = 3, // not clicked inside object
    
    TEST = 5,
    DEL_FROM_UNITS_LIST = 5,
    
    PICK_COUNTRY = 6,
    START_GAME = 7,

    PROV_SWITCH_TAB = 8,
    OPEN_PROV = 9,
    OPEN_COUNTRY = 10,
    OPEN_UNIT = 11,

    DECLARE_WAR = 12,
    OPEN_OFFER_PEACE = 13,
    SEND_PEACE_OFFER = 14,
    OPEN_WAR_WINDOW = 15,

    DELETE_PROV_FROM_PEACE = 16,
    OPEN_PEACE_OFFERT = 17,
    REJECT_PEACE_OFFER = 18,
    ACCEPT_PEACE_OFFER = 19,
    OPEN_MY_COUNTRY = 20,
    OPEN_COUNTRY_FROM_PROV = 21,
    CREATE_UNIT = 22,
    SET_SPEED = 23,
    SIDEBAR_LEFTC = 24,
    ROOM_PICK_COUNTRY = 25,
    ROOM_START_GAME = 26,
    MERGE_UNITS = 27,
    START_AS_SPECTATOR = 28,
    UNIT_SIZE = 29,
    
    CLOSE_WINDOW
};


