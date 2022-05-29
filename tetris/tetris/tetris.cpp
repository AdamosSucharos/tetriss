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
unsigned char* pField = nullptr;

int nScreenWidth = 80;
int nScreenHeight = 30;

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
                    if (tvary[nTetromino][pi] == L'X' && pField[fi] != 0)
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

    pField = new unsigned char[sirka * vyska];
    for (int x = 0; x < sirka; x++)
        for (int y = 0; y < vyska; y++)
            pField[y * sirka + x] = (x == 0 || x == sirka - 1 || y == vyska - 1) ? 9 : 0;

    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool bGameOver = false;

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = sirka / 2;
    int nCurrentY = 0;

    bool bKey[4];
    bool bRotateHold = false;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;

    std::vector<int> vLines;

    while (!bGameOver) {
        // Game Timing
        std::this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // Input
        for (int k = 0; k < 4; k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // Game Logic
        nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
        nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

        if (bKey[3]) {

            nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        }
        else bRotateHold = false;

        if (bForceDown) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;
            else {
                // Lock
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tvary[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                            pField[(nCurrentY + py) * sirka + (nCurrentX + px)] = nCurrentPiece + 1;
                nPieceCount++;
                if (nPieceCount % 10 == 0)
                    if (nSpeed >= 10) nSpeed--;
                // Check for lines
                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < vyska - 1) {
                        bool bLine = true;
                        for (int px = 1; px < sirka - 1; px++)
                            bLine &= (pField[(nCurrentY + py) * sirka + px]) != 0;
                        if (bLine) {
                            for (int px = 1; px < sirka - 1; px++)
                                pField[(nCurrentY + py) * sirka + px] = 8;

                            vLines.push_back(nCurrentY + py);
                        }
                    }
                nScore += 25;
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;
                // Choose next piece
                nCurrentX = sirka / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // game over
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
            nSpeedCounter = 0;
        }
        // Render Output


        // Draw Field
        for (int x = 0; x < sirka; x++)
            for (int y = 0; y < vyska; y++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * sirka + x]];

        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tvary[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

        // Draw Score
        swprintf_s(&screen[2 * nScreenWidth + vyska + 6], 16, L"SKÓRE: %8d", nScore);

        if (!vLines.empty()) {
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(400ms);

            for (auto& v : vLines)
                for (int px = 1; px < sirka - 1; px++) {
                    for (int py = v; py > 0; py--)
                        pField[py * sirka + px] = pField[(py - 1) * sirka + px];
                    pField[px] = 0;
                }
            vLines.clear();
        }

        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }
    // GG no re
    CloseHandle(hConsole);
    cout << "Game Over! Final Score:" << nScore << endl;
    system("pause");
    return 0;
}