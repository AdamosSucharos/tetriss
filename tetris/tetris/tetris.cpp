// tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
using namespace std;


wstring tvary[7];
int sirka = 12;
int vyska = 18;
unsigned char* pole = nullptr;

int sirkaObrazu = 80;
int vyskaObrazu = 30;

int Rotate(int px, int py, int r) {
    switch (r % 4) {
    case 0: return py * 4 + px;         // 0   
    case 1: return 12 + py - (px * 4); // 90  
    case 2: return 15 - (py * 4) - px;  // 180 
    case 3: return 3 - py + (px * 4);   // 270  
    }
    return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++) {
            int pi = Rotate(px, py, nRotation);

            int fi = (nPosY + py) * sirka + (nPosX + px);
            if (nPosX + px >= 0 && nPosX + px < sirka) {
                if (nPosY + py >= 0 && nPosY + py < vyska) {
                    if (tvary[nTetromino][pi] == L'X' && pole[fi] != 0)
                        return false;
                }
            }
        }
    return true;
}

int main()
{
    tvary[0].append(L"..X.");
    tvary[0].append(L"..X.");
    tvary[0].append(L"..X.");
    tvary[0].append(L"..X.");

    tvary[1].append(L"..X.");
    tvary[1].append(L".XX.");
    tvary[1].append(L".X..");
    tvary[1].append(L"....");

    tvary[2].append(L".X..");
    tvary[2].append(L".XX.");
    tvary[2].append(L"..X.");
    tvary[2].append(L"....");

    tvary[3].append(L"....");
    tvary[3].append(L".XX.");
    tvary[3].append(L".XX.");
    tvary[3].append(L"....");

    tvary[4].append(L"..X.");
    tvary[4].append(L".XX.");
    tvary[4].append(L"..X.");
    tvary[4].append(L"....");

    tvary[5].append(L"....");
    tvary[5].append(L".XX.");
    tvary[5].append(L"..X.");
    tvary[5].append(L"..X.");

    tvary[6].append(L"....");
    tvary[6].append(L".XX.");
    tvary[6].append(L".X..");
    tvary[6].append(L".X..");
    
    pole = new unsigned char[sirka * vyska];
    for (int x = 0; x < sirka; x++)
        for (int y = 0; y < vyska; y++)
            pole[y * sirka + x] = (x == 0 || x == sirka - 1 || y == vyska - 1) ? 9 : 0;

    wchar_t* screen = new wchar_t[sirkaObrazu * vyskaObrazu];
    for (int i = 0; i < sirkaObrazu * vyskaObrazu; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool koniecHry = false;

    int kusy = 0;
    int otacanie = 0;
    int xx = sirka / 2;
    int yy = 0;

    bool kluc[4];
    bool drzanieOtocky = false;

    int rychlost = 20;
    int meracRychlosti = 0;
    bool dole = false;
    int pocitacKusov = 0;
    int skore = 0;

    std::vector<int> vLines;

    while (!koniecHry) {
        // Game Timing
        std::this_thread::sleep_for(50ms);
        meracRychlosti++;
        dole = (meracRychlosti == rychlost);

        // Input
        for (int k = 0; k < 4; k++)
            kluc[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // Game Logic
        xx -= (kluc[1] && DoesPieceFit(kusy, otacanie, xx - 1, yy)) ? 1 : 0;
        xx += (kluc[0] && DoesPieceFit(kusy, otacanie, xx + 1, yy)) ? 1 : 0;
        yy += (kluc[2] && DoesPieceFit(kusy, otacanie, xx, yy + 1)) ? 1 : 0;

        if (kluc[3]) {

            otacanie += (!drzanieOtocky && DoesPieceFit(kusy, otacanie + 1, xx, yy)) ? 1 : 0;
            drzanieOtocky = true;
        }
        else drzanieOtocky = false;

        if (dole) {
            if (DoesPieceFit(kusy, otacanie, xx, yy + 1))
                yy++;
            else {
                // Lock
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tvary[kusy][Rotate(px, py, otacanie)] == L'X')
                            pole[(yy + py) * sirka + (xx + px)] = kusy + 1;
                pocitacKusov++;
                if (pocitacKusov % 10 == 0)
                    if (rychlost >= 10) rychlost--;
                // Check for lines
                for (int py = 0; py < 4; py++)
                    if (yy + py < vyska - 1) {
                        bool bLine = true;
                        for (int px = 1; px < sirka - 1; px++)
                            bLine &= (pole[(yy + py) * sirka + px]) != 0;
                        if (bLine) {
                            for (int px = 1; px < sirka - 1; px++)
                                pole[(yy + py) * sirka + px] = 8;

                            vLines.push_back(yy + py);
                        }
                    }
                skore += 25;
                if (!vLines.empty()) skore += (1 << vLines.size()) * 100;
                // Choose next piece
                xx = sirka / 2;
                yy = 0;
                otacanie = 0;
                kusy = rand() % 7;

                // game over
                koniecHry = !DoesPieceFit(kusy, otacanie, xx, yy);
            }
            meracRychlosti = 0;
        }
        // Render Output


        // Draw Field
        for (int x = 0; x < sirka; x++)
            for (int y = 0; y < vyska; y++)
                screen[(y + 2) * sirkaObrazu + (x + 2)] = L" ABCDEFG=#"[pole[y * sirka + x]];

        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tvary[kusy][Rotate(px, py, otacanie)] == L'X')
                    screen[(yy + py + 2) * sirkaObrazu + (xx + px + 2)] = kusy + 65;

        // Draw Score
        swprintf_s(&screen[2 * sirkaObrazu + vyska + 6], 16, L"SKÓRE: %8d", skore);

        if (!vLines.empty()) {
            WriteConsoleOutputCharacter(hConsole, screen, sirkaObrazu * vyskaObrazu, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(400ms);

            for (auto& v : vLines)
                for (int px = 1; px < sirka - 1; px++) {
                    for (int py = v; py > 0; py--)
                        pole[py * sirka + px] = pole[(py - 1) * sirka + px];
                    pole[px] = 0;
                }
            vLines.clear();
        }

        WriteConsoleOutputCharacter(hConsole, screen, sirkaObrazu * vyskaObrazu, { 0,0 }, &dwBytesWritten);
    }
    // GG no re
    CloseHandle(hConsole);
    cout << "Game Over! Final Score:" << skore << endl;
    system("pause");
    return 0;
}