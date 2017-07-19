//******************************************************************************
// Simple AC Remote
//
// Copyright 2017 Gutierrez PS
//
// http://github.com/gutierrezps/simple-ac-remote
//
//******************************************************************************

#include <IRremote.h>
#include <IRremoteInt.h>

#include "IRData.hpp"
#include "IRDecoder.hpp"
#include "IRSender.hpp"
#include "IRRawAnalyzer.hpp"

const struct
{
    char buttonLevel;
    char buttonOff;
    char led1;
    char led2;
    char led3;
    char ledBlink;
    char irSensor;
}
g_pins = {10, 11, 4, 5, 6, 7, 2};

IRrecv g_irRecv(g_pins.irSensor);
IRsend g_irSender;                  // IR LED connected on pin 3

char g_ACLevel = 0;                 // 0: off, 1-3: cooling level
char g_sendCode = 0;
const char g_remoteQty = 6;

IRData * g_codes[5][4];

void dumper();

void level(char l);
void level0();
void level1();
void level2();
void level3();
void sendLevel(uint8_t data[][17]);

void setup()
{
    pinMode(g_pins.irSensor, INPUT);
    pinMode(g_pins.led1, OUTPUT);
    pinMode(g_pins.led2, OUTPUT);
    pinMode(g_pins.led3, OUTPUT);
    pinMode(g_pins.ledBlink, OUTPUT);
    pinMode(g_pins.buttonOff, INPUT_PULLUP);
    pinMode(g_pins.buttonLevel, INPUT_PULLUP);

    Serial.begin(115200);

    // Enter dumper mode if level button is held on startup
    if(digitalRead(g_pins.buttonLevel) == LOW)
    {
        digitalWrite(g_pins.ledBlink, HIGH);
        delay(100);
        while(digitalRead(g_pins.buttonOff) == LOW) delay(10);
        digitalWrite(g_pins.ledBlink, LOW);
        delay(100);
        dumper();
    }
}

void loop()
{
    if(digitalRead(g_pins.buttonLevel) == LOW)
    {
        delay(100);
        while(digitalRead(g_pins.buttonLevel) == LOW);

        if(++g_ACLevel == 4) g_ACLevel = 1;
        g_sendCode = 1;
    }

    if(digitalRead(g_pins.buttonOff) == LOW)
    {
        delay(100);
        while(digitalRead(g_pins.buttonOff) == LOW);
        g_ACLevel = 0;
        g_sendCode = 1;
    }

    if(g_sendCode)
    {
        Serial.print("sending ");
        Serial.println(g_ACLevel, DEC);

        digitalWrite(g_pins.led1, LOW);
        digitalWrite(g_pins.led2, LOW);
        digitalWrite(g_pins.led3, LOW);

        level(g_ACLevel);

        delay(400);
        g_sendCode = 0;
    }
}


void level(char l)
{
    switch(l)
    {
        case 3:
            digitalWrite(g_pins.led3, HIGH);
        case 2:
            digitalWrite(g_pins.led2, HIGH);
        case 1:
            digitalWrite(g_pins.led1, HIGH);
            break;
    }

    switch(l)
    {
        case 3: level3(); break;
        case 2: level2(); break;
        case 1: level1(); break;
        default: level0();
    }
}


void level0()
{
    uint8_t controls[g_remoteQty][17] = {
        {   112,    // number of bits
            // data
            0xC4,0xD3,0x64,0x80,0x00,0x04,0xC0,0xE0,0x1C,0x00,0x00,0x00,0x00,0xEE,
            2, 0    // protocol id, isRepeated
        },
        {   48,
            0xB2,0x4D,0x7B,0x84,0xE0,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            4, 1
        },
        {
            28,
            0x88,0xC0,0x05,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        },
        {
            97,
            0xFF,0x00,0xFF,0x00,0xFF,0x00,0xDF,0x20,0xEB,0x14,0x54,0xAB,0x00,0x00,
            3, 0
        },
        {
            35,
            0x82,0x10,0x00,0x0A,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            6, 0
        },
        {
            44,
            0x20,0x49,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        }
    };

    sendLevel(controls);
}

void level1()
{
    uint8_t controls[g_remoteQty][17] = {
        {   112,
            0xC4,0xD3,0x64,0x80,0x00,0x24,0xC0,0xE0,0x1C,0x00,0x00,0x00,0x00,0xDE,
            2, 0
        },
        {   48,
            0xB2,0x4D,0xBF,0x40,0x40,0xBF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            4, 1
        },
        {
            28,
            0x88,0x00,0x95,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        },
        {
            97,
            0xFF,0x00,0xFF,0x00,0xFF,0x00,0x9F,0x60,0xEB,0x14,0x54,0xAB,0x00,0x00,
            3, 0
        },
        {
            35,
            0x92,0x10,0x00,0x0A,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            6, 0
        },
        {
            44,
            0x28,0x49,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        }
    };

    sendLevel(controls);
}

void level2()
{
    uint8_t controls[g_remoteQty][17] = {
        {   112,
            0xC4,0xD3,0x64,0x80,0x00,0x24,0xC0,0x10,0x1C,0x00,0x00,0x00,0x00,0x3E,
            2, 0
        },
        {   48,
            0xB2,0x4D,0xBF,0x40,0x50,0xAF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            4, 1
        },
        {
            28,
            0x88,0x08,0x85,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        },
        {
            97,
            0xFF,0x00,0xFF,0x00,0xBF,0x40,0x9F,0x60,0x1B,0xE4,0x54,0xAB,0x00,0x00,
            3, 0
        },
        {
            35,
            0x92,0xE0,0x00,0x0A,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            6, 0
        },
        {
            44,
            0x28,0x48,0x00,0x00,0x00,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        }
    };

    sendLevel(controls);
}

void level3()
{
    uint8_t controls[g_remoteQty][17] = {
        {   112,
            0xC4,0xD3,0x64,0x80,0x00,0x24,0xC0,0x90,0x1C,0x00,0x00,0x00,0x00,0xBE,
            2, 0
        },
        {   48,
            0xB2,0x4D,0xBF,0x40,0x70,0x8F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            4, 1
        },
        {
            28,
            0x88,0x08,0x75,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        },
        {
            97,
            0xFF,0x00,0xFF,0x00,0xBF,0x40,0x9F,0x60,0x9B,0x64,0x54,0xAB,0x00,0x00,
            3, 0
        },
        {
            35,
            0x92,0x60,0x00,0x0A,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            6, 0
        },
        {
            44,
            0x28,0x47,0x00,0x00,0x00,0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            1, 0
        }
    };

    sendLevel(controls);
}

void sendLevel(uint8_t data[][17])
{
    IRData irData;
    char control, i;

    for(control = 0; control < g_remoteQty; control++)
    {
        irData.nBits = data[control][0];
        for(i = 0; i < irData.Length(); i++)
        {
            irData.data[i] = data[control][i+1];
        }
        irData.protocol = g_irProtocols.GetProtocol(data[control][15]);
        irData.isRepeated = data[control][16];
        irData.isValid = true;

        irData.ToString();


        digitalWrite(g_pins.ledBlink, HIGH);
        sendIR(g_irSender, irData);
        delay(50);
        digitalWrite(g_pins.ledBlink, LOW);
        delay(50);
    }
}



void dumper()
{
    char blinkStatus = 1, received = 0;
    unsigned long blinkTimer = 0;
    decode_results irRawData;

    Serial.println("dumper mode");

    g_irRecv.enableIRIn();

    while(1)
    {
        IRData data;

        if(g_irRecv.decode(&irRawData)) received = 1;

        digitalWrite(g_pins.ledBlink, blinkStatus);

        if(!received)
        {
            if(++blinkTimer > 500)
            {
                blinkStatus = blinkStatus ? 0 : 1;
                blinkTimer = 0;
            }

            delay(1);
            continue;   // return to loop beginning
        }

        dumpRaw(&irRawData, 0);
        analyze(&irRawData);
        decodeIR(&irRawData, data, 1);

        if(data.isValid)    // i.e. known protocol
        {
            data.ToString();

            digitalWrite(g_pins.led1, HIGH);
            delay(50);
            digitalWrite(g_pins.led1, LOW);
            delay(50);
            digitalWrite(g_pins.led1, HIGH);
            delay(50);
            digitalWrite(g_pins.led1, LOW);
        }
        else
        {
            digitalWrite(g_pins.led1, HIGH);
            delay(500);
            digitalWrite(g_pins.led1, LOW);
        }

        g_irRecv.resume();      // get another code
        received = 0;
        blinkTimer = 0;
        blinkStatus = 1;

        digitalWrite(g_pins.led1, LOW);
    }

}
