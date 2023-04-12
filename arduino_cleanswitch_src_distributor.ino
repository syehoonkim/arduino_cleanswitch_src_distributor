#define RX 7
#define TX 8
#include <SoftwareSerial.h>

SoftwareSerial rserial(RX, TX);

uint8_t pByte[155];

bool shouldRead = true;

uint8_t cmd[] = "@ X:0/0,X/1,X/2,X/3,X/4,X/5,X/6,X/7,X/8,X/9,X/A,X/B,X\r";
char targetSrc;

int beginComm()
{
    rserial.write('\r');
    Serial.println("First carriage return sent.");

    rserial.readBytes(pByte, 2);
    pByte[2] = '\0';
    Serial.println((char *)pByte);

    rserial.write("@ ?\r", 4);
    Serial.println("Enable Report Command sent. Ready to do Source Distribution.");
}

void setup()
{
    Serial.begin(115200);
    rserial.begin(9600);
    rserial.listen();
    Serial.println("Arduino CleanSwitch Source Distributor Controller Started.");
    beginComm();
}

void loop()
{
    rserial.readBytesUntil('\n', pByte, 9);
    Serial.print("Received: ");
    Serial.println((char *)pByte);

    if ((char)pByte[0] == 'S' && (char)pByte[1] == ':' && (char)pByte[2] == '0')
    {
        if (shouldRead)
        {
            shouldRead = false;
            targetSrc = pByte[5];

            Serial.print("Target Source changed to: ");
            Serial.println(((targetSrc > 'A') ? (targetSrc - 'A' + 11) : (targetSrc - '0' + 1))); // Why is it showing funny? Anyway, working fine.

            Serial.println("Changing Routing...");
            cmd[8] = cmd[12] = cmd[16] = cmd[20] = cmd[24] = cmd[28] = cmd[32] = cmd[36] = cmd[40] = cmd[44] = cmd[48] = cmd[52] = targetSrc;
            rserial.write(cmd, 54);
            Serial.println("Sending Source Distribution Command executed.");

            int readbytes = rserial.readBytes(pByte, 152);
            pByte[readbytes] = '\0';
            Serial.print((char *)pByte);

            for (int i = 0; i < 152; i++)
            {
                pByte[i] = '0';
            }

            rserial.write('\r');
            Serial.println("Carriage Return sent.");

            rserial.readBytes(pByte, 1);
            pByte[1] = '\0';
            Serial.println((char *)pByte);
        }
        shouldRead = true;
        Serial.println("Source Distributed.");
    }
    else
    {
        Serial.println("Waiting for routing command...\n");
    }
}