#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_OTHER_CARD_VALUES_IN_DECK 20
#define MAX_CARDS_ON_PLAYER_DECK_OR_HAND 129
#define MAX_LENGTH_OF_WORD_COLOR 10
#define MAX_LENGTH_LINE_INPUT 512
#define MAX_LENGTH_FIRST_INPUT 128
#define FIRST_INPUT_LENGTH 3

struct gameData {
    int players;
    int cauldrons;
    int greenCards;
    int greenCardValue;
    int otherCardValue;
    int otherCardValues[MAX_OTHER_CARD_VALUES_IN_DECK];
    int activePlayer;
    int explosion;
};

struct cardInfo {
    int value;
    char color[MAX_LENGTH_OF_WORD_COLOR];
};

struct players {
    int playerId;
    cardInfo playerCardsOnHand[MAX_CARDS_ON_PLAYER_DECK_OR_HAND];
    cardInfo playerCardsOnDeck[MAX_CARDS_ON_PLAYER_DECK_OR_HAND];
    int handSize;
    int deckSize;
};

struct cauldrons {
    int cauldronId;
    cardInfo cardsOnPile[MAX_CARDS_ON_PLAYER_DECK_OR_HAND];
    int cauldronSize;
};


/*
 * FUNKCJE ODPOWIEDZIALNE ZA GENEROWANIE TALII KART
 * */

//Dodawanie zielonych kart do nowej talii
void greenCardGeneration (gameData deck, cardInfo myDeck[])
{
    char temp[MAX_LENGTH_OF_WORD_COLOR] = "green";
    for (int i = 0; i < deck.greenCards; i++)
    {
        myDeck[i].value = deck.greenCardValue;
        for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
            myDeck[i].color[j] = temp[j];
    }
}
//Dodawanie reszty kart do nowej talii
void restOfCardsGeneration (gameData deck, cardInfo myDeck[], char colors[][MAX_LENGTH_OF_WORD_COLOR])
{
    int index = deck.greenCards;
    for (int i = 0; i < deck.cauldrons; i++)
    {
        for (int j = 0; j < deck.otherCardValue; j++)
        {
            myDeck[index].value = deck.otherCardValues[j];
            for (int _ = 0; _ < MAX_LENGTH_OF_WORD_COLOR; _++)
                myDeck[index].color[_] = colors[i][_];
            index++;
        }
    }
}

//Generowanie talii kart używając funkcji do generowania osobno zielonych kart oraz reszty kart
void deckGeneration (gameData deck, cardInfo generatedDeck[])
{
    char cardColors[6][MAX_LENGTH_OF_WORD_COLOR] = {"blue", "red", "violet", "yellow", "white", "black"};
    greenCardGeneration(deck, generatedDeck);
    restOfCardsGeneration(deck, generatedDeck, cardColors);
    int deckSize = deck.greenCards + (deck.cauldrons * deck.otherCardValue);

    FILE *fptr;
    fptr = fopen("deck.txt", "w");
    for (int i = 0; i < deckSize; i++) {
        fprintf (fptr, "%i", generatedDeck[i].value);
        fprintf (fptr, " ");
        fprintf (fptr, "%s", generatedDeck[i].color);
        fprintf (fptr, " ", generatedDeck[i]);
    }
    fclose(fptr);
}

//Wczytywanie danych z konsoli
void insertDeckData (gameData &deck)
{
    FILE *fptr;
    fptr = fopen("data.txt", "r");
    clearerr(fptr);

    if (fptr != NULL) {
        fscanf(fptr, "%i", &deck.players);
        fscanf(fptr, "%i", &deck.cauldrons);
        fscanf(fptr, "%i", &deck.greenCards);
        fscanf(fptr, "%i", &deck.greenCardValue);
        fscanf(fptr, "%i", &deck.otherCardValue);
        for (int i = 0; i < deck.otherCardValue; i++)
            fscanf(fptr, "%i", &deck.otherCardValues[i]);
        fscanf(fptr, "%i", &deck.explosion);
    }
    else
        std::cout << "Nie udało się otworzyć pliku!";
    fclose(fptr);
}

/*
 * FUNKCJE ODPOWIEDZIALNE ZA TASOWANIE KART
 */

//Zamienianie karty na podstawie indeksu wygenerowanego losowo
void swapCard  (int x, int temp, cardInfo generatedDeck[])
{
    int tempValue = 0;
    char tempColor[MAX_LENGTH_OF_WORD_COLOR];
    tempValue = generatedDeck[x].value;
    for (int i = 0; i < MAX_LENGTH_OF_WORD_COLOR; i++)
        tempColor[i] = generatedDeck[x].color[i];
    generatedDeck[x].value = generatedDeck[temp].value;
    for (int i = 0; i < MAX_LENGTH_OF_WORD_COLOR; i++)
        generatedDeck[x].color[i] = generatedDeck[temp].color[i];
    generatedDeck[temp].value = tempValue;
    for (int i = 0; i < MAX_LENGTH_OF_WORD_COLOR; i++)
        generatedDeck[temp].color[i] = tempColor[i];

}

//Generowanie losowego indeksu dla kazdej karty oraz zamienianie ich
void shuffleDeck (int deckSize ,cardInfo generatedDeck[])
{
    int temp = 0;

    for (int i = 0; i < deckSize; i++)
    {
        temp = (i + (rand() % deckSize - i));
        swapCard (i, temp, generatedDeck);
    }
}

/*
 * FUNKCJE ODPOWIEDZIALNE ZA ROZDAWANIE KART GRACZOM
 * */

//Funkcja służąca do rozdawania kart
void cardsDeal (players info[], cardInfo deck[], gameData myDeck, int deckSize)
{
    int *cardsOnHandCounter = new int[myDeck.players];
    for (int i = 0; i < myDeck.players; i++)
    {
        cardsOnHandCounter[i] = 0;
    }
    //Dodaje do tablicy pomocniczej ile kart ma na ręce poszczególny gracz i dodaje dla każdego gracza karty
    for (int i = 0; i < deckSize; i++)
    {
        info[i % myDeck.players].playerCardsOnHand[cardsOnHandCounter[i % myDeck.players]].value = deck[i].value;
        for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
            info[i % myDeck.players].playerCardsOnHand[cardsOnHandCounter[i % myDeck.players]].color[j] = deck[i].color[j];
        cardsOnHandCounter[i % myDeck.players]++;
    }
    //Przepisuję do tablicy graczy ile ma kart na ręce
    for (int i = 0; i < myDeck.players; i++)
    {
        info[i].handSize = cardsOnHandCounter[i];
    }
    delete[] cardsOnHandCounter;
}

//Wypisuje karty kazdego gracza po rozdaniu wygenerowanej karty i ile kart leży na stosie
void printDeckAfterCardsDeal (gameData myDeck, players info[])
{
    std::cout << "active player = " << "1" << std::endl;
    std::cout << "players number = " << myDeck.players << std::endl;
    std::cout << "explosive threshold = " << myDeck.explosion << std::endl;
    for (int i = 0; i < myDeck.players; i++)
    {
        std::cout << i + 1 << " player hand cards: ";
        for (int j = 0; j < info[i].handSize; j++)
        {
            std::cout << info[i].playerCardsOnHand[j].value << " " << info[i].playerCardsOnHand[j].color << " ";
        }
        std::cout << std::endl;
        std::cout << i + 1 << " player deck cards:";
        std::cout << std::endl;
    }
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        std::cout << i + 1 << " pile cards:" << std::endl;
    }
}

//Wypisywanie kazdej karty znajdujacej sie w talii
void printDeck (cardInfo deck[], int deckSize)
{
    for (int i = 0; i < deckSize; i++)
    {
        std::cout << deck[i].value << " " << deck[i].color << " ";
    }
    std::cout << std::endl;
}

/*
 * ___________________________________________________________________
 * FUNKCJE ODPOWIEDZIALNE ZA WCZYTYWANIE DANYCH PODCZAS ŁADOWANIA GRY
 * ___________________________________________________________________
 * */


//Na podstawie pierwszych linijek wczytanych danych wpisuje do struktury gameData kto jest aktywnym graczem, ile jest graczy oraz jaka liczba wywoluje eksplozje kociolka
void activeAndHowMany(gameData &loadedGame)
{
    int howMuchPlayers, activePlayer, explode;
    char firstInput[FIRST_INPUT_LENGTH][MAX_LENGTH_FIRST_INPUT];
    for (int i = 0; i < FIRST_INPUT_LENGTH; i++)
    {
        for (int j = 0; j < MAX_LENGTH_FIRST_INPUT; j++)
        {
            firstInput[i][j] = 0;
        }
    }
    FILE *fptr;
    fptr = fopen("input.txt", "r");
    clearerr(fptr);

    int index = 0;
    if (fptr != NULL) {
        while(index < 3)
        {
            fgets(firstInput[index], MAX_LENGTH_FIRST_INPUT, fptr);
            index++;
        }
    }
    else
        std::cout << "Nie udało się otworzyć pliku!";
    fclose(fptr);

    //Dodawanie do zmiennych liczby graczy, aktywnego gracza oraz liczbe wywołującą wybuch kociołka
    for ( int i = 0; i < MAX_LENGTH_FIRST_INPUT; i++)
    {
        if (firstInput[0][i] >= '0' && firstInput[0][i] <= '9')
            activePlayer = (int)(firstInput[0][i] - '0');
        if (firstInput[0][i - 1] >= '0' && firstInput[0][i - 1] <= '9' && i < MAX_LENGTH_FIRST_INPUT-1 && firstInput[0][i] >= '0' && firstInput[0][i] <= '9' && i > 0) {
            char temp[2];
            temp[0] = firstInput[0][i - 1];
            temp[1] = firstInput[0][i];
            activePlayer = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
        }
        if (firstInput[1][i] >= '0' && firstInput[1][i] <= '9')
            howMuchPlayers = (int)(firstInput[1][i] - '0');
        if (firstInput[1][i - 1] >= '0' && firstInput[1][i - 1] <= '9' && i < MAX_LENGTH_FIRST_INPUT-1 && firstInput[1][i] >= '0' && firstInput[1][i] <= '9' && i > 0) {
            char temp[2];
            temp[0] = firstInput[1][i - 1];
            temp[1] = firstInput[1][i];
            howMuchPlayers = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
        }
        if (firstInput[2][i] >= '0' && firstInput[2][i] <= '9' && i > 0)
            explode = (int) (firstInput[2][i] - '0');
        if (firstInput[2][i - 1] >= '0' && firstInput[2][i - 1] <= '9' && i < MAX_LENGTH_FIRST_INPUT-1 && firstInput[2][i] >= '0' && firstInput[2][i] <= '9' && i > 0) {
            char temp[2];
            temp[0] = firstInput[2][i - 1];
            temp[1] = firstInput[2][i];
            explode = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
        }
        if (firstInput[2][i - 2] >= '0' && firstInput[2][i - 2] <= '9' && firstInput[2][i - 1] >= '0' && firstInput[2][i - 1] <= '9' && i < MAX_LENGTH_FIRST_INPUT-2 && firstInput[2][i] >= '0' && firstInput[2][i] <= '9' && i > 0) {
            char temp[3];
            temp[0] = firstInput[2][i-2];
            temp[1] = firstInput[2][i-1];
            temp[2] = firstInput[2][i];
            explode = (int((temp[0] - '0') * 100) + int((temp[1] - '0') * 10) + int(temp[2] - '0'));
        }

    }
    loadedGame.players = howMuchPlayers;
    loadedGame.activePlayer = activePlayer;
    loadedGame.explosion = explode;
}

//Na podstawie tablicy struktur oraz podanych indeksow wpisuje kolor karty do danej karty na ręce gracza
void cardColorToHandArray (players playersData[], char **inputLines, int playersIndex, int cardIndex, int x, int y)
{
    int index = y;
    int index2 = 0;
    while (inputLines[x][index] != ' ' && index2 != MAX_LENGTH_OF_WORD_COLOR-1 && index != MAX_LENGTH_LINE_INPUT-1)
    {
        playersData[playersIndex].playerCardsOnHand[cardIndex].color[index2] = inputLines[x][index];
        index++;
        index2++;
    }
}

//Na podstawie tablicy struktur oraz podanych indeksow wpisuje kolor karty do danej karty na stoliku gracza
void cardColorToDeckArray (players playersData[], char **inputLines, int playersIndex, int cardIndex, int x, int y)
{
    int index = y;
    int index2 = 0;
    while (inputLines[x][index] != ' ' && index2 != MAX_LENGTH_OF_WORD_COLOR-1 && index != MAX_LENGTH_LINE_INPUT-1)
    {
        playersData[playersIndex].playerCardsOnDeck[cardIndex].color[index2] = inputLines[x][index];
        index++;
        index2++;
    }
}

//Pobieranie reszty danych z wczytywania i zapisywanie do tymczasowej zmiennej
void gettingRestInput (gameData loadedGame, players playersData[]) {

    int maxArraySize = 2 * loadedGame.players;
    char **inputLines = new char *[maxArraySize];
    for (int i = 0; i < maxArraySize; i++) {
        inputLines[i] = new char[MAX_LENGTH_LINE_INPUT];
    }
    char trash[3][MAX_LENGTH_LINE_INPUT];

    for (int i = 0; i < maxArraySize; i++)
    {
        for (int j = 0; j < MAX_LENGTH_LINE_INPUT; j++)
        {
            inputLines[i][j] = 0;
        }
    }
    FILE *fptr;
    fptr = fopen("input.txt", "r");
    clearerr(fptr);

    int index = 0;
    int index2 = 0;
    if (fptr != NULL) {
        while(index < maxArraySize + 3)
        {
            if (index <= 2)
                fgets(trash[index], MAX_LENGTH_LINE_INPUT, fptr);
            if (index > 2)
            {
                fgets(inputLines[index2], MAX_LENGTH_LINE_INPUT, fptr);
                index2++;
            }
            index++;
        }
    }
    else
        std::cout << "Nie udało się otworzyć pliku!";
    clearerr(fptr);
    fclose(fptr);

    //Wpisywanie wczytanych danych do tablicy struktur graczy
    int playerHandIndex = 0;
    int playerDeckIndex = 0;
    playersData[playerHandIndex].playerId = playerHandIndex + 1;
    for (int i = 0; i < 2 * loadedGame.players; i++)
    {
        int cardHandIndex = 0;
        int cardDeckIndex = 0;
        for (int j = 1; j < MAX_LENGTH_LINE_INPUT; j++)
        {
            char temp[2];
            if (i % 2 == 0)
            {
                if (inputLines[i][j] >= '0' && inputLines[i][j] <= '9' && inputLines[i][j + 1] >= '0' && inputLines[i][j + 1] <= '9' && j < MAX_LENGTH_LINE_INPUT-1)
                {
                    temp[0] = inputLines[i][j];
                    temp[1] = inputLines[i][j + 1];
                    playersData[playerHandIndex].playerCardsOnHand[cardHandIndex].value = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
                    cardColorToHandArray(playersData, inputLines, playerHandIndex, cardHandIndex, i, j+3);
                    cardHandIndex++;
                }
                else if (inputLines[i][j] >= '0' && inputLines[i][j] <= '9' && inputLines[i][j-1] == ' ')
                {
                    playersData[playerHandIndex].playerCardsOnHand[cardHandIndex].value = int(inputLines[i][j] - '0');
                    cardColorToHandArray(playersData, inputLines, playerHandIndex, cardHandIndex, i, j+2);
                    cardHandIndex++;
                }
            }
            else if (i % 2 != 0)
            {
                if (inputLines[i][j] >= '0' && inputLines[i][j] <= '9' && inputLines[i][j + 1] >= '0' && inputLines[i][j + 1] <= '9' && j < MAX_LENGTH_LINE_INPUT-1)
                {
                    temp[0] = inputLines[i][j];
                    temp[1] = inputLines[i][j + 1];
                    playersData[playerDeckIndex].playerCardsOnDeck[cardDeckIndex].value = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
                    cardColorToDeckArray(playersData, inputLines, playerDeckIndex, cardDeckIndex, i, j+3);
                    cardDeckIndex++;
                }
                else if (inputLines[i][j] >= '0' && inputLines[i][j] <= '9' && inputLines[i][j-1] == ' ')
                {
                    playersData[playerDeckIndex].playerCardsOnDeck[cardDeckIndex].value = int(inputLines[i][j] - '0');
                    cardColorToDeckArray(playersData, inputLines, playerDeckIndex, cardDeckIndex, i, j+2);
                    cardDeckIndex++;
                }
            }
        }
        if (i % 2 == 0)
            playerHandIndex++;
        else if (i % 2 != 0)
            playerDeckIndex++;
    }

    for (int i = 0; i < maxArraySize; i++)
        delete[] inputLines[i];
    delete[] inputLines;
}

//Liczenie ile linijek się wczytało oraz liczenie na tej podstawie ile jest kociołków
void howMuchCauldrons(gameData &loadedGame, char **input)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < MAX_LENGTH_LINE_INPUT; j++)
        {
            input[i][j] = 0;
        }
    }
    char trash[128][MAX_LENGTH_LINE_INPUT];

    for (int i = 0; i < (loadedGame.players * 2) + 3; i++){
        for (int j = 0; j < MAX_LENGTH_LINE_INPUT; j++)
        {
            trash[i][j] = 0;
        }
    }

    int maxArraySize = (loadedGame.players * 2) + 3;
    FILE *fptr;
    fptr = fopen("input.txt", "r");
    clearerr(fptr);

    int index = 0, index2 = 0;
    if (fptr != NULL) {
        while(!feof(fptr))
        {
            if (index < maxArraySize)
                fgets(trash[index], MAX_LENGTH_LINE_INPUT, fptr);
            if (index > maxArraySize-1) {
                fgets(input[index2], MAX_LENGTH_LINE_INPUT, fptr);
                index2++;
            }
            index++;
        }
    }
    else
        std::cout << "Nie udało się otworzyć pliku!";
    clearerr(fptr);
    fclose(fptr);

    int cauldronsCounter = 0;

    for (int i = 0; i < 6; i++)
    {
        if (input[i][0] != 0)
            cauldronsCounter++;
    }
    loadedGame.cauldrons = cauldronsCounter;
}

//Dodawanie koloru do tablicy struktur kociołków do danej karty
void cardColorToCauldronArray (cauldrons cauldronsData[], char **inputLines, int playersIndex, int cardIndex, int x, int y)
{
    int index = y;
    int index2 = 0;
    while (inputLines[x][index] != ' ' && index2 != MAX_LENGTH_OF_WORD_COLOR-1 && index != MAX_LENGTH_LINE_INPUT-1)
    {
        cauldronsData[playersIndex].cardsOnPile[cardIndex].color[index2] = inputLines[x][index];
        index++;
        index2++;
    }
}

//Dodawanie wartości kart do tablicy struktur kociołka
void cauldronsDataFromInput (char **input, cauldrons cauldronsData[], gameData loadedGame)
{
    for (int i = 0; i < loadedGame.cauldrons; i++)
    {
        int cardIndex = 0;
        cauldronsData[i].cauldronId = i+1;
        for (int j = 0; j < MAX_LENGTH_LINE_INPUT; j++)
        {
            char temp[2];
            if (input[i][j] >= '0' && input[i][j] <= '9' && input[i][j + 1] >= '0' && input[i][j + 1] <= '9' && j < MAX_LENGTH_LINE_INPUT-1)
            {
                temp[0] = input[i][j];
                temp[1] = input[i][j + 1];
                cauldronsData[i].cardsOnPile[cardIndex].value = (int(temp[0] - '0') * 10) + int(temp[1] - '0');
                cardColorToCauldronArray(cauldronsData, input, i, cardIndex, i, j+3);
                cardIndex++;
            }
            else if (input[i][j] >= '0' && input[i][j] <= '9' && input[i][j-1] == ' ')
            {
                cauldronsData[i].cardsOnPile[cardIndex].value = int(input[i][j] - '0');
                cardColorToCauldronArray(cauldronsData, input, i, cardIndex, i, j+2);
                cardIndex++;
            }
        }
    }
}

/*
 * FUNKCJE ODPOWIEDZIALNE ZA WYLICZANIE ILE KART MA GRACZ NA RĘCE, A ILE MA ICH PRZED SOBĄ
 */

//Liczenie ile jest kart na ręce i przed graczem oraz dodawanie tego do każdego gracza
void fridayThe13th3bLoadGame (gameData loadedGame, players playersData[], cauldrons cauldronsData[]) {
    int *counterHand = new int[loadedGame.players];
    int *counterDeck = new int[loadedGame.players];
    for (int i = 0; i < loadedGame.players; i++) {
        counterDeck[i] = 0;
        counterHand[i] = 0;
    }
    int *counterCauldrons = new int[loadedGame.cauldrons];
    for (int i = 0; i < loadedGame.cauldrons; i++)
        counterCauldrons[i] = 0;
    for (int i = 0; i < loadedGame.players; i++) {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++) {
            if (playersData[i].playerCardsOnHand[j].value != 0)
                counterHand[i]++;
            if (playersData[i].playerCardsOnDeck[j].value != 0)
                counterDeck[i]++;
        }
    }
    for (int i = 0; i < loadedGame.players; i++) {
        playersData[i].handSize = counterHand[i];
        playersData[i].deckSize = counterDeck[i];
    }

    for (int i = 0; i < loadedGame.cauldrons; i++)
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++) {
            if (cauldronsData[i].cardsOnPile[j].value != 0)
                counterCauldrons[i]++;
        }

    for (int i = 0; i < loadedGame.cauldrons; i++)
        cauldronsData[i].cauldronSize = counterCauldrons[i];

    delete[] counterHand;
    delete[] counterDeck;
    delete[] counterCauldrons;
}

//Wyswietlanie ilosci kart graczy i kociolkow
void printFridayThe13th3bLoadGame (gameData loadedgame, cauldrons cauldronsData[], players playersData[])
{
    for (int i = 0; i < loadedgame.players; i++)
    {
        std::cout << i+1 << " player has " << playersData[i].handSize << " cards on hand" << std::endl;
        std::cout << i+1 << " player has " << playersData[i].deckSize << " cards in front of him" << std::endl;
    }
    for (int i = 0; i < loadedgame.cauldrons; i++)
    {
        std::cout  << "there are " << cauldronsData[i].cauldronSize << " cards on " << i+1 << " pile" << std::endl;
    }
}

/*
 * SPRAWDZANIE CZY ZGADZAJĄ SIĘ WARTOŚCI ZIELONYCH KART
 */

//Wyswietlanie komunikatu bledu jezeli wystepuja rozne wartosci zielonych kart
void printFridayThe13th4aCheckGreenCards (int counter, int value)
{
    if (value == -1)
        std::cout << "Different green cards values occurred" << std::endl;
    if (value == 0)
        std::cout << "Green cards does not exist" << std::endl;
    else if (value > 0)
        std::cout << "Found " << counter << " green cards, all with " << value << " value" << std::endl;
}

//Funkcja sprawdzajaca czy wystepuja rozne wartosci zielonych kart
bool fridayThe13th4aCheckGreenCards (gameData loadedgame, cauldrons cauldronsData[], players playersData[])
{
    int greenCardValue = 0, counter = 0;
    for (int i = 0; i < loadedgame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnHand[j].color[0] == 'g')
            {
                if (greenCardValue == 0 || playersData[i].playerCardsOnHand[j].value == greenCardValue) {
                    greenCardValue = playersData[i].playerCardsOnHand[j].value;
                    counter++;
                } else
                    greenCardValue = -1;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'g')
            {
                if (greenCardValue == 0 || playersData[i].playerCardsOnDeck[j].value == greenCardValue) {
                    greenCardValue = playersData[i].playerCardsOnDeck[j].value;
                    counter++;
                } else
                    greenCardValue = -1;
            }
        }
    }
    for (int i = 0; i < loadedgame.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'g')
            {
                if (greenCardValue == 0 || cauldronsData[i].cardsOnPile[j].value == greenCardValue) {
                    greenCardValue = cauldronsData[i].cardsOnPile[j].value;
                    counter++;
                }
                else
                    greenCardValue = -1;
            }
    }
    if (greenCardValue > 0)
        return true;
    printFridayThe13th4aCheckGreenCards(counter, greenCardValue);
    return false;
}

/*
 * SPRAWDZANIE CZY ZGADZA SIĘ ILOŚĆ RESZTY KART
 */

//Sprawdzanie ilosci kart kazdego koloru na rece gracza
void fridayThe13th4bCheckGreenCardsPl1 (gameData loadedgame, players playersData[], int cards[])
{
    for (int i = 0; i < loadedgame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnHand[j].color[0] == 'b' && playersData[i].playerCardsOnHand[j].color[2] == 'u')
                cards[0]++;
            if (playersData[i].playerCardsOnHand[j].color[0] == 'r')
                cards[1]++;
            if (playersData[i].playerCardsOnHand[j].color[0] == 'v')
                cards[2]++;
            if (playersData[i].playerCardsOnHand[j].color[0] == 'y')
                cards[3]++;
            if (playersData[i].playerCardsOnHand[j].color[0] == 'w')
                cards[4]++;
            if (playersData[i].playerCardsOnHand[j].color[0] == 'b' && playersData[i].playerCardsOnHand[j].color[2] == 'a')
                cards[5]++;
        }
    }
}

//Sprawdzanie ilosci kart kazdego koloru na stoliku gracza
void fridayThe13th4bCheckGreenCardsPl2 (gameData loadedgame, players playersData[], int cards[])
{
    for (int i = 0; i < loadedgame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'u')
                cards[0]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'r')
                cards[1]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'v')
                cards[2]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'y')
                cards[3]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'w')
                cards[4]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'a')
                cards[5]++;
        }
    }
}

//Sprawdzanie ilosci kart kazdego koloru na stosie kart
void fridayThe13th4bCheckGreenCardsCo(gameData loadedgame, cauldrons cauldronsData[], int cards[])
{
    for (int i = 0; i < loadedgame.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'u')
                cards[0]++;
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'r')
                cards[1]++;
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'v')
                cards[2]++;
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'y')
                cards[3]++;
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'w')
                cards[4]++;
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'a')
                cards[5]++;
        }
    }
}

//Wyswietlanie komunikatu bledu jezeli w grze znajduja sie rozni ilosci kart danych kolorow
void printFridayThe13th4bCheckGreenCards(int cards[])
{
    char cardColors[6][MAX_LENGTH_OF_WORD_COLOR] = {"blue", "red", "violet", "yellow", "white", "black"};
    bool equal = true;
    int temp = 0;
    for (int i = 0; i < 6; i++)
    {
        if (cards[i] != 0)
        {
            temp = cards[i];
            break;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        if (cards[i] != temp && cards[i] != 0)
            equal = false;
    }
    if (equal)
    {
        std::cout << "The number cards of all colors is equal: " << temp << std::endl;
    }
    else if (!equal)
    {
        std::cout << "At least two colors with a different number of cards were found: " << std::endl;
        for (int i = 0; i < 6; i++)
        {
            if ((i == 0) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
            if ((i == 1) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
            if ((i == 2) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
            if ((i == 3) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
            if ((i == 4) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
            if ((i == 5) && (cards[i] != 0))
                std::cout << cardColors[i] << " cards are " << cards[i] << std::endl;
        }
    }

}

//Sprawdzanie czy wszystkich kart jest po rowno jezeli tak zwraca prawde
bool checkFridayThe13th4bCheckGreenCards(int cards[])
{
    char cardColors[6][MAX_LENGTH_OF_WORD_COLOR] = {"blue", "red", "violet", "yellow", "white", "black"};
    bool equal = true;
    int temp = 0;
    for (int i = 0; i < 6; i++)
    {
        if (cards[i] != 0)
        {
            temp = cards[i];
            break;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        if (cards[i] != temp && cards[i] != 0)
            equal = false;
    }
    if (equal)
        return true;
    return false;
}

//Glowna funkcja wywolujaca prace wszystkich innych funkcji sprawdzajacych czy ilosc kart jest rowna
bool fridayThe13th4bCheckGreenCards (gameData loadedGame, cauldrons cauldronsData[], players playersData[])
{
    int cards[6];
    for (int i = 0; i < 6; i++)
        cards[i] = 0;
    fridayThe13th4bCheckGreenCardsPl1(loadedGame, playersData, cards);
    fridayThe13th4bCheckGreenCardsPl2(loadedGame, playersData, cards);
    fridayThe13th4bCheckGreenCardsCo(loadedGame, cauldronsData, cards);
    if(checkFridayThe13th4bCheckGreenCards(cards))
        return true;
    printFridayThe13th4bCheckGreenCards(cards);
    return false;
}

/*
 * SPRAWDZANIE CZY ZGADZAJĄ SIĘ WARTOŚCI RESZTY KART
 */

//Liczenie ilosci kart danego koloru i wartosci na stosach kart
void fridayThe13th5CheckCardsValuePlC (gameData loadGame, cauldrons cauldronsData[], players playersData[], int cards[][MAX_OTHER_CARD_VALUES_IN_DECK], int card[])
{
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'u')
            {
                cards[0][card[0]] = cauldronsData[i].cardsOnPile[j].value;
                card[0]++;
            }
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'r')
            {
                cards[1][card[1]] = cauldronsData[i].cardsOnPile[j].value;
                card[1]++;
            }
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'v')
            {
                cards[2][card[2]] = cauldronsData[i].cardsOnPile[j].value;
                card[2]++;
            }
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'y')
            {
                cards[3][card[3]] = cauldronsData[i].cardsOnPile[j].value;
                card[3]++;
            }
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'w')
            {
                cards[4][card[4]] = cauldronsData[i].cardsOnPile[j].value;
                card[4]++;
            }
            if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'a')
            {
                cards[5][card[5]] = cauldronsData[i].cardsOnPile[j].value;
                card[5]++;
            }
        }
    }
}

//Liczenie ilosci kart danego koloru i wartosci na stoliku gracza
void fridayThe13th5CheckCardsValuePlD (gameData loadGame, cauldrons cauldronsData[], players playersData[], int cards[][MAX_OTHER_CARD_VALUES_IN_DECK], int card[])
{
    for (int i = 0; i < loadGame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'u')
            {
                cards[0][card[0]] = playersData[i].playerCardsOnDeck[j].value;
                card[0]++;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'r')
            {
                cards[1][card[1]] = playersData[i].playerCardsOnDeck[j].value;
                card[1]++;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'v')
            {
                cards[2][card[2]] = playersData[i].playerCardsOnDeck[j].value;
                card[2]++;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'y')
            {
                cards[3][card[3]] = playersData[i].playerCardsOnDeck[j].value;
                card[3]++;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'w')
            {
                cards[4][card[4]] = playersData[i].playerCardsOnDeck[j].value;
                card[4]++;
            }
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'a')
            {
                cards[5][card[5]] = playersData[i].playerCardsOnDeck[j].value;
                card[5]++;
            }
        }
    }
}

//Liczenie ilosci kart danego koloru i wartosci na rece gracza
void fridayThe13th5CheckCardsValuePlH (gameData loadGame, cauldrons cauldronsData[], players playersData[], int cards[][MAX_OTHER_CARD_VALUES_IN_DECK], int card[])
{
    for (int i = 0; i < loadGame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnHand[j].color[0] == 'b' && playersData[i].playerCardsOnHand[j].color[2] == 'u')
            {
                cards[0][card[0]] = playersData[i].playerCardsOnHand[j].value;
                card[0]++;
            }
            if (playersData[i].playerCardsOnHand[j].color[0] == 'r')
            {
                cards[1][card[1]] = playersData[i].playerCardsOnHand[j].value;
                card[1]++;
            }
            if (playersData[i].playerCardsOnHand[j].color[0] == 'v')
            {
                cards[2][card[2]] = playersData[i].playerCardsOnHand[j].value;
                card[2]++;
            }
            if (playersData[i].playerCardsOnHand[j].color[0] == 'y')
            {
                cards[3][card[3]] = playersData[i].playerCardsOnHand[j].value;
                card[3]++;
            }
            if (playersData[i].playerCardsOnHand[j].color[0] == 'w')
            {
                cards[4][card[4]] = playersData[i].playerCardsOnHand[j].value;
                card[4]++;
            }
            if (playersData[i].playerCardsOnHand[j].color[0] == 'b' && playersData[i].playerCardsOnHand[j].color[2] == 'a')
            {
                cards[5][card[5]] = playersData[i].playerCardsOnHand[j].value;
                card[5]++;
            }
        }
    }
}

//Jezeli wartosci sa rozne wypisuje komunikat bledu
void printFridayThe13th5CheckCardsValue (int cards[][MAX_OTHER_CARD_VALUES_IN_DECK])
{
    char cardColors[6][MAX_LENGTH_OF_WORD_COLOR] = {"blue", "red", "violet", "yellow", "white", "black"};
    bool equal = true;
    int cardsIndex[6] = {-1, -1, -1, -1, -1, -1};
    for (int i = 0; i < 6; i++) {
        if (cards[i][MAX_OTHER_CARD_VALUES_IN_DECK-1] == 0) {
            cardsIndex[i] = i;
        }
    }
    int temp[MAX_OTHER_CARD_VALUES_IN_DECK];
    for (int i = 0; i < 6; i++) {
        if (cardsIndex[i] == -1) {
            for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++) {
                temp[j] = cards[i][j];
            }
            break;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++)
        {
            if (cards[i][j] == 0)
                continue;
            else if (cards[i][j] != temp[j])
                equal = false;
        }
    }
    if (equal)
    {
        std::cout << "The values of cards of all colors are identical: " << std::endl;
        for (int i = 0; i < MAX_OTHER_CARD_VALUES_IN_DECK; i++) {
            if (temp[i] == 0)
                continue;
            std::cout << temp[i] << " ";
        }
    }
    else
    {
        std::cout << "The values of cards of all colors are not identical:" << std::endl;
        for (int i = 0; i < 6; i++){
            if (i == cardsIndex[0] || i == cardsIndex[1] || i == cardsIndex[2] || i == cardsIndex[3] || i == cardsIndex[4] || i == cardsIndex[5])
                break;
            std::cout << cardColors[i] << " cards values: ";
            for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++){
                if (cards[i][j] == 0)
                    continue;
                else
                    std::cout << cards[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
}

//Sprawdzenie czy wartosci kart kazdego koloru sa rowne
bool checkFridayThe13th5CheckCardsValue (int cards[][MAX_OTHER_CARD_VALUES_IN_DECK]) {
    char cardColors[6][MAX_LENGTH_OF_WORD_COLOR] = {"blue", "red", "violet", "yellow", "white", "black"};
    bool equal = true;
    int cardsIndex[6] = {-1, -1, -1, -1, -1, -1};
    for (int i = 0; i < 6; i++) {
        if (cards[i][19] == 0) {
            cardsIndex[i] = i;
        }
    }
    int temp[MAX_OTHER_CARD_VALUES_IN_DECK];
    for (int i = 0; i < 6; i++) {
        if (cardsIndex[i] == -1) {
            for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++) {
                temp[j] = cards[i][j];
            }
            break;
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++) {
            if (cards[i][j] == 0)
                continue;
            else if (cards[i][j] != temp[j])
                equal = false;
        }
    }
    if (equal)
        return true;
    return false;
}

//Glowna funkcja wywolujaca prace innych funkcji w celu sprawdzenia czy wartosci kart roznych kolorow sa rowne
bool fridayThe13th5CheckCardsValue (gameData loadGame, cauldrons cauldronsData[], players playersData[])
{
    int card[6] = {0, 0, 0, 0, 0, 0};
    int cards[6][MAX_OTHER_CARD_VALUES_IN_DECK];
    for (int i = 0; i < 6; i++)
    {
        for(int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++)
        {
            cards[i][j] = 0;
        }
    }
    fridayThe13th5CheckCardsValuePlH(loadGame, cauldronsData, playersData, cards, card);
    fridayThe13th5CheckCardsValuePlD(loadGame, cauldronsData, playersData, cards, card);
    fridayThe13th5CheckCardsValuePlC(loadGame, cauldronsData, playersData, cards, card);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < MAX_OTHER_CARD_VALUES_IN_DECK; j++) {
            for (int p = 0; p < MAX_OTHER_CARD_VALUES_IN_DECK - j - 1; p++) {
                if (cards[i][p] > cards[i][p + 1]) {
                    int temp = cards[i][p];
                    cards[i][p] = cards[i][p + 1];
                    cards[i][p + 1] = temp;
                }
            }
        }
    }
    if (checkFridayThe13th5CheckCardsValue(cards))
        return true;
    printFridayThe13th5CheckCardsValue(cards);
    return false;
}

/*
 * SPRAWDZANIE CZY ZGADZA SIĘ STAN GRY
 */

//Sprawdzanie czy wystepuja rozne kolory kart na kociolkach
void differentColors (gameData loadGame, cauldrons cauldronsData[], int color[])
{
    char *temp = new char[loadGame.cauldrons];
    for (int i = 0; i < loadGame.cauldrons; i++)
        temp[i] = '0';
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        for(int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if(cauldronsData[i].cardsOnPile[j].color[0] == 'g')
                continue;
            else if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'a') {
                temp[i] = 'B';
            }
            else if(cauldronsData[i].cardsOnPile[j].color[0] != 'g' && cauldronsData[i].cardsOnPile[j].color[0] >= 'a' && cauldronsData[i].cardsOnPile[j].color[0] <= 'z')
                temp[i] = cauldronsData[i].cardsOnPile[j].color[0];
        }
    }
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        for(int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if(cauldronsData[i].cardsOnPile[j].color[0] == 'g')
                continue;
            else if (cauldronsData[i].cardsOnPile[j].color[0] == 'b' && cauldronsData[i].cardsOnPile[j].color[2] == 'a') {
                if (temp[i] != 'B')
                    color[i] = 0;
            }
            else if(cauldronsData[i].cardsOnPile[j].color[0] != 'g' && cauldronsData[i].cardsOnPile[j].color[0] >= 'a' && cauldronsData[i].cardsOnPile[j].color[0] <= 'z') {
                if (temp[i] != cauldronsData[i].cardsOnPile[j].color[0])
                    color[i] = 0;
            }
        }
    }
    delete[] temp;
}

//Sprawdzanie czy suma wartosci w kociolku sie zgadza czy nie powinien wybuchnac szybciej
void explosiveTreshold (gameData loadGame, cauldrons cauldronsData[], int explosive[])
{
    int *boom = new int[loadGame.cauldrons];
    for (int i = 0; i < loadGame.cauldrons; i++)
        boom[i] = 0;
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            boom[i] += cauldronsData[i].cardsOnPile[j].value;
        }
    }
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        if (boom[i] >= loadGame.explosion)
            explosive[i] = 0;
    }
    delete[] boom;
}

//Sprawdzanie czy gracze maja dobra ilosc kart na rece
bool checkAmountOfCards (gameData loadGame, players playersData[])
{
    for (int i = 1; i < loadGame.players; i++)
    {
        if (i - 1 == loadGame.activePlayer - 1) {
            if (abs(playersData[i - 1].handSize - playersData[i].handSize) > 2)
                return false;
        }
        else
        if (abs(playersData[i - 1].handSize - playersData[i].handSize) > 1)
            return false;
    }
    return true;
}

//Wyswietlanie komunikatu bledu jezeli takowy wystepuje
void printCheckGameState (gameData loadGame, int color[], int explosive[], bool amount)
{
    bool okColor = true, okExplosive = true;
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        if (color[i] != 1)
            okColor = false;
        if (explosive[i] != 1)
            okExplosive = false;
    }
    if (amount && okColor && okExplosive)
        std::cout << "Current state of the game is ok" << std::endl;
    else
    {
        if (!amount)
            std::cout << "The number of players cards on hand is wrong" << std::endl;
        if (!okColor)
        {
            for (int i = 0; i < loadGame.cauldrons; i++)
            {
                if (color[i] == 0)
                    std::cout << "Two different colors were found on the " << i + 1 << " pile" << std::endl;
            }
        }
        if (!okExplosive)
        {
            for (int i = 0; i < loadGame.cauldrons; i++)
            {
                if (explosive[i] == 0)
                    std::cout << "Pile number " << i + 1 << " should explode earlier" << std::endl;
            }
        }
    }
}

//Jezeli nie wystepuje zaden blad zwraca prawde
bool checkCheckGameState (gameData loadGame, int color[], int explosive[], bool amount)
{
    bool okColor = true, okExplosive = true;
    for (int i = 0; i < loadGame.cauldrons; i++)
    {
        if (color[i] != 1)
            okColor = false;
        if (explosive[i] != 1)
            okExplosive = false;
    }
    if (amount && okColor && okExplosive)
        return true;
    return false;
}

//Jezeli nie napotkalo na zaden blad zwraca prawde w przeciwnym wypadku wypisuje blad
bool checkGameState (gameData loadGame, cauldrons cauldronsData[], players playersData[])
{
    int *color = new int[loadGame.cauldrons];
    int *explosive = new int[loadGame.cauldrons];
    bool amount;
    for (int i = 0; i < loadGame.cauldrons; i++) {
        color[i] = 1;
        explosive[i] = 1;
    }
    differentColors(loadGame, cauldronsData, color);
    explosiveTreshold(loadGame, cauldronsData, explosive);
    amount = checkAmountOfCards(loadGame, playersData);
    if (checkCheckGameState(loadGame, color, explosive, amount))
        return true;

    delete[] color;
    delete[] explosive;
    printCheckGameState(loadGame, color, explosive, amount);
    return false;

    delete[] color;
    delete[] explosive;
}

/*
 * WYKONANIE PROSTEGO POSUNIĘCIA
 */

//Wybieranie pierwszej karty z reki gracza
void whichCard (gameData loadGame, players playersData[], int &cardValue, char cardColor[])
{
    cardValue = playersData[loadGame.activePlayer - 1].playerCardsOnHand[0].value;
    for (int i = 0; i < MAX_LENGTH_OF_WORD_COLOR; i++)
        cardColor[i] = playersData[loadGame.activePlayer - 1].playerCardsOnHand[0].color[i];
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++)
    {
        if (i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND - 1)
            playersData[loadGame.activePlayer - 1].playerCardsOnHand[i] = playersData[loadGame.activePlayer - 1].playerCardsOnHand[i + 1];
        else {
            playersData[loadGame.activePlayer - 1].playerCardsOnHand[i].value = 0;
            for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
                playersData[loadGame.activePlayer - 1].playerCardsOnHand[i].color[j] = 0;
        }
    }
}

//Przypisywanie wybranej karty do stosu
void addToPile (gameData loadGame, cauldrons cauldronsData[], int cardValue, char cardColor[]) {
    bool isEmpty = true;
    for (int i = 0; i < loadGame.cauldrons; i++) {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++) {
            if (cauldronsData[i].cardsOnPile[j].value != 0 && cauldronsData[i].cardsOnPile[j].color[0] != 'g')
                isEmpty = false;
        }
    }
    if (isEmpty || cardColor[0] == 'g') {
        for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++) {
            if (cauldronsData[0].cardsOnPile[i].value == 0) {
                cauldronsData[0].cardsOnPile[i].value = cardValue;
                for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
                    cauldronsData[0].cardsOnPile[i].color[j] = cardColor[j];
                break;
            }
        }
    } else {
        int index = -1;
        for (int i = 0; i < loadGame.cauldrons; i++) {
            for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++) {
                if (cardColor[0] == cauldronsData[i].cardsOnPile[j].color[0] &&
                    cardColor[2] == cauldronsData[i].cardsOnPile[j].color[2] && cauldronsData[i].cardsOnPile[j].color[0] != 'g')
                    index = i;
            }
        }
        if (index != -1) {
            for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++) {
                if (cauldronsData[index].cardsOnPile[i].value == 0 ) {
                    cauldronsData[index].cardsOnPile[i].value = cardValue;
                    for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
                        cauldronsData[index].cardsOnPile[i].color[j] = cardColor[j];
                    break;
                }
            }
        } else {
            int temp = -1;
            for (int i = 0; i < loadGame.cauldrons; i++) {
                bool empty = true;
                for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++) {
                    if (cauldronsData[i].cardsOnPile[j].value != 0 && cauldronsData[i].cardsOnPile[j].color[0] != 'g')
                        empty = false;
                }
                if (empty)
                    temp = i;
            }
            for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++) {
                if (cauldronsData[temp].cardsOnPile[i].value == 0) {
                    cauldronsData[temp].cardsOnPile[i].value = cardValue;
                    for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
                        cauldronsData[temp].cardsOnPile[i].color[j] = cardColor[j];
                    break;
                }
            }
        }
    }
}

//Wypisywanie stanu gry po prostym posunięciu
void printEasyMove (gameData loadedGame, cauldrons cauldronsData[], players playersData[])
{
    std::cout << "active player = " << loadedGame.activePlayer << std::endl;
    std::cout << "players number = " << loadedGame.players << std::endl;
    std::cout << "explosion threshold = " << loadedGame.explosion << std::endl;
    int indexHand = 0;
    int indexDeck = 0;
    for (int i = 0; i < loadedGame.players * 2; i++)
    {
        if (i % 2 == 0)
            std::cout << indexHand + 1 << " player hand cards: ";
        else if (i % 2 != 0)
            std::cout << indexDeck + 1 << " player deck cards: ";
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (i % 2 == 0)
            {
                if (playersData[indexHand].playerCardsOnHand[j].value <= 0 || playersData[indexHand].playerCardsOnHand[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                    continue;
                std::cout << playersData[indexHand].playerCardsOnHand[j].value << " ";
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                {
                    if (playersData[indexHand].playerCardsOnHand[j].color[p] < 'a' || playersData[indexHand].playerCardsOnHand[j].color[p] > 'z')
                        continue;
                    std::cout << playersData[indexHand].playerCardsOnHand[j].color[p];
                }
                std::cout << " ";
            }
            if (i % 2 != 0)
            {
                if (playersData[indexDeck].playerCardsOnDeck[j].value <= 0 || playersData[indexDeck].playerCardsOnDeck[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                    continue;
                std::cout << playersData[indexDeck].playerCardsOnDeck[j].value << " ";
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                {
                    if (playersData[indexDeck].playerCardsOnDeck[j].color[p] < 'a' || playersData[indexDeck].playerCardsOnDeck[j].color[p] > 'z')
                        continue;
                    std::cout << playersData[indexDeck].playerCardsOnDeck[j].color[p];
                }
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        if (i % 2 == 0)
            indexHand++;
        if (i % 2 != 0)
            indexDeck++;
    }
    for (int i = 0; i < loadedGame.cauldrons; i++)
    {
        std::cout << i + 1 << " pile cards: ";
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (cauldronsData[i].cardsOnPile[j].value < 0 || cauldronsData[i].cardsOnPile[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                continue;
            else {
                if (cauldronsData[i].cardsOnPile[j].value == 0)
                    continue;
                else {
                    std::cout << cauldronsData[i].cardsOnPile[j].value << " ";
                    for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++) {
                        if (cauldronsData[i].cardsOnPile[j].color[p] == '\n' || cauldronsData[i].cardsOnPile[j].color[p] == 0)
                            continue;
                        std::cout << cauldronsData[i].cardsOnPile[j].color[p];
                    }
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
    }
}

//Pobiera numer stosu ktory powinien wybuchnac
void cauldronExplosion (gameData loadGame, cauldrons cauldronsData[], players playersData[], int &pileId)
{
    int *explosive = new int[loadGame.cauldrons];
    for (int i = 0; i < loadGame.cauldrons; i++)
        explosive[i] = 1;
    explosiveTreshold(loadGame, cauldronsData, explosive);
    for (int i = 0; i < loadGame.cauldrons; i++)
        if (explosive[i] == 0)
            pileId = i;
    delete[] explosive;
}

//Dodaje karty po wybuchu kociolka do danego gracza
void addCardsAfterBoom (gameData loadGame, cauldrons cauldronsData[], players playersData[], int pileId)
{
    int cardId = 0;
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++)
    {
        if (playersData[loadGame.activePlayer- 1].playerCardsOnDeck[i].value == 0) {
            playersData[loadGame.activePlayer - 1].playerCardsOnDeck[i].value = cauldronsData[pileId].cardsOnPile[cardId].value;
            for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
                playersData[loadGame.activePlayer - 1].playerCardsOnDeck[i].color[j] = cauldronsData[pileId].cardsOnPile[cardId].color[j];
            cardId++;
        }
    }
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++){
        cauldronsData[pileId].cardsOnPile[i].value = 0;
        for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
            cauldronsData[pileId].cardsOnPile[i].color[j] = 0;
    }
}

//Wykonanie prostego posuniecia
void easyMove (gameData &loadGame, cauldrons cauldronsData[], players playersData[])
{
    int pileId = -1;
    int cardValue = 0;
    char cardColor[MAX_LENGTH_OF_WORD_COLOR];
    whichCard (loadGame, playersData, cardValue, cardColor);
    addToPile (loadGame, cauldronsData, cardValue, cardColor);
    cauldronExplosion(loadGame, cauldronsData, playersData, pileId);
    if (pileId != -1)
        addCardsAfterBoom(loadGame, cauldronsData, playersData, pileId);
    if (loadGame.activePlayer == loadGame.players)
        loadGame.activePlayer = 1;
    else
        loadGame.activePlayer++;
//    printEasyMove(loadGame, cauldronsData, playersData);
}

/*
 * OBSŁUGA ZAKOŃCZENIA RUNDY
 */

//Sprawdza czy dany gracz posiada najwiecej kart danego koloru
void checkImmunity (gameData loadGame, players playersData[], int **colorCounter, char color[][MAX_LENGTH_OF_WORD_COLOR], bool **immunity)
{
    for (int i = 1; i < 7; i++)
    {
        int max = 0;
        int maxId = -1;
        int j = 0;
        while (j < loadGame.players)
        {
            int temp = 0;
            temp = colorCounter[j][i];
            if (temp > max){
                max = temp;
                maxId = j;
            }
            else if (temp == max){
                maxId = -1;
            }
            j++;
        }
        if (maxId != -1)
            immunity[maxId][i] = true;
    }
}

//Liczy ile poszczegolnych kart ma gracz
void countingColors(gameData loadGame, players playersData[], int **colorCounter, char color[][MAX_LENGTH_OF_WORD_COLOR])
{
    for (int i = 0; i < loadGame.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'g')
                colorCounter[i][0]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'u')
                colorCounter[i][1]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'r')
                colorCounter[i][2]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'v')
                colorCounter[i][3]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'y')
                colorCounter[i][4]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'w')
                colorCounter[i][5]++;
            if (playersData[i].playerCardsOnDeck[j].color[0] == 'b' && playersData[i].playerCardsOnDeck[j].color[2] == 'a')
                colorCounter[i][6]++;
        }
    }
}

//Wypisuje stan po zakonczeniu gry
void printEndOfRound (gameData loadGame, char color[][MAX_LENGTH_OF_WORD_COLOR], int **colorCounter, bool **immunity)
{
    int *score = new int[loadGame.players];
    for (int i = 0; i < loadGame.players; i++)
        score[i] = 0;
    for (int i = 1; i < 7; i++)
    {
        for (int j = 0; j < loadGame.players; j++)
        {
            if (immunity[j][i])
                std::cout << "Na kolor " << color[i] << " odporny jest gracz " << j+1 << std::endl;
        }
    }
    for (int i = 0; i < loadGame.players; i++){
        for (int j = 0; j < 7; j++){
            if (immunity[i][j])
                continue;
            else {
                if (j == 0)
                    score[i] += 2 * colorCounter[i][j];
                else
                    score[i] += colorCounter[i][j];
            }
        }
    }
    for (int i = 0; i < loadGame.players; i++)
    {
        std::cout << "Wynik gracza " << i+1 << " = " << score[i] << std::endl;
    }
    delete[] score;
}

//Obsluga konca rundy
void endOfRound (gameData loadGame, cauldrons cauldronsData[], players playersData[MAX_LENGTH_OF_WORD_COLOR])
{
    char color[7][MAX_LENGTH_OF_WORD_COLOR] = {"green", "blue", "red", "violet", "yellow", "white", "black"};
    int **colorCounter = new int* [loadGame.players];
    for (int i = 0; i < loadGame.players; i++)
        colorCounter[i] = new int[7];
    bool **immunity = new bool* [loadGame.players];
    for (int i = 0; i < loadGame.players; i++)
        immunity[i] = new bool[7];
    for (int i = 0; i < loadGame.players; i++) {
        for (int j = 0; j < 7; j++) {
            colorCounter[i][j] = 0;
            immunity[i][j] = false;
        }
    }
    countingColors(loadGame, playersData, colorCounter, color);
    checkImmunity(loadGame, playersData, colorCounter, color, immunity);
    printEndOfRound(loadGame, color, colorCounter, immunity);

    for (int i = 0; i < loadGame.players; i++){
        delete[] colorCounter[i];
        delete[] immunity[i];
    }
    delete[] immunity;
    delete[] colorCounter;
}

//Generowanie pliku ze stanem gry
void generateGameDataFile (gameData myDeck, cauldrons cauldronsData[], players playersData[])
{
    FILE *fptr;
    fptr = fopen("input.txt", "w");
    fprintf(fptr, "active player = %i\n", myDeck.activePlayer);
    fprintf(fptr, "players number = %i\n", myDeck.players);
    fprintf(fptr, "explosion threshold = %i\n", myDeck.explosion);
    int indexHand = 0, indexDeck = 0;
    for (int i = 0; i < myDeck.players * 2; i++)
    {
        if (i % 2 == 0)
            fprintf(fptr, "%i player hand cards: ", indexHand + 1);
        else if (i % 2 != 0)
            fprintf(fptr, "%i player deck cards: ", indexDeck + 1);
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (i % 2 == 0)
            {
                if (playersData[indexHand].playerCardsOnHand[j].value <= 0 || playersData[indexHand].playerCardsOnHand[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                    continue;
                fprintf(fptr, "%i ", playersData[indexHand].playerCardsOnHand[j].value);
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                {
                    if (playersData[indexHand].playerCardsOnHand[j].color[p] < 'a' || playersData[indexHand].playerCardsOnHand[j].color[p] > 'z')
                        continue;
                    fprintf(fptr, "%c", playersData[indexHand].playerCardsOnHand[j].color[p]);
                }
                fprintf(fptr, " ");
            }
            if (i % 2 != 0)
            {
                if (playersData[indexDeck].playerCardsOnDeck[j].value <= 0 || playersData[indexDeck].playerCardsOnDeck[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                    continue;
                fprintf(fptr, "%i ", playersData[indexDeck].playerCardsOnDeck[j].value);
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                {
                    if (playersData[indexDeck].playerCardsOnDeck[j].color[p] < 'a' || playersData[indexDeck].playerCardsOnDeck[j].color[p] > 'z')
                        continue;
                    fprintf(fptr, "%c", playersData[indexDeck].playerCardsOnDeck[j].color[p]);
                }
                fprintf(fptr, " ");
            }
        }
        fprintf(fptr, "\n");
        if (i % 2 == 0)
            indexHand++;
        if (i % 2 != 0)
            indexDeck++;
    }
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        fprintf(fptr, "%i pile cards: ", i + 1);
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (cauldronsData[i].cardsOnPile[j].value < 0 || cauldronsData[i].cardsOnPile[j].value > MAX_OTHER_CARD_VALUES_IN_DECK)
                continue;
            else {
                if (cauldronsData[i].cardsOnPile[j].value == 0)
                    continue;
                else {
                    fprintf(fptr, "%i ", cauldronsData[i].cardsOnPile[j].value);
                    for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++) {
                        if (cauldronsData[i].cardsOnPile[j].color[p] == '\n' || cauldronsData[i].cardsOnPile[j].color[p] == 0)
                            continue;
                        fprintf(fptr, "%c", cauldronsData[i].cardsOnPile[j].color[p]);
                    }
                    fprintf(fptr, " ");
                }
            }
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
}

//Arbiter sprawdzajacy czy stan gry sie zgadza, jezeli nie wypisuje rodzaj bledu
bool isGameOk (gameData loadedGame, cauldrons cauldronsData[], players playersData[])
{
    if (fridayThe13th4aCheckGreenCards(loadedGame, cauldronsData, playersData) && fridayThe13th4bCheckGreenCards(loadedGame, cauldronsData, playersData) &&
        fridayThe13th5CheckCardsValue(loadedGame, cauldronsData, playersData) && checkGameState(loadedGame, cauldronsData, playersData))
        return true;
    return false;
}

//Funkcja odpowiedzialna za czyszczenie zmiennych i zapisywanie nowych danych wczytanych z pliku
void loadGameFromFile (gameData &myDeck, players playersStats[], cauldrons cauldronsData[])
{
    myDeck.activePlayer = 0;
    for (int i = 0; i < myDeck.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            playersStats[i].playerCardsOnDeck[j].value = 0;
            playersStats[i].playerCardsOnHand[j].value = 0;
            for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
            {
                playersStats[i].playerCardsOnDeck[j].color[p] = 0;
                playersStats[i].playerCardsOnHand[j].color[p] = 0;
            }
        }
    }
    for (int i = 0; i < myDeck.cauldrons; i++)
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            cauldronsData[i].cardsOnPile[j].value = 0;
            for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                cauldronsData[i].cardsOnPile[j].color[p] = 0;
        }
    activeAndHowMany(myDeck);
    gettingRestInput(myDeck, playersStats);
    char **input = new char* [6];
    for (int i = 0; i < 6; i++)
    {
        input[i] = new char[MAX_LENGTH_LINE_INPUT];
    }
    howMuchCauldrons(myDeck, input);
    cauldronsDataFromInput (input, cauldronsData, myDeck);

    for (int i = 0; i < 6; i++){
        delete[] input[i];
    }
    delete[] input;
}

void whichCards (gameData myDeck, players playersStats[], cauldrons cauldronsData[], cardInfo smallest[], cardInfo smallestCopy[])
{
    int index = 1;
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++)
    {
        for (int j = 0; j < playersStats[myDeck.activePlayer - 1].handSize - i - 1; j++)
        {
            if (playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j].value > playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j + 1].value && playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j].value != 0 && playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j+1].value != 0)
            {
                cardInfo temp = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j];
                playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j + 1];
                playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j + 1] = temp;
            }
        }
    }
    smallest[0] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[0];
    smallestCopy[0] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[0];
    for (int i = 1; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++)
    {
        if (playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i].value == smallest[0].value) {
            smallest[index] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i];
            smallestCopy[index] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i];
            index++;
        }
    }
}

void whichPile (gameData myDeck, players playersStats[], cauldrons cauldronsData[], cardInfo piles[])
{
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        piles[i].value = 0;
        for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
            piles[i].color[j] = 0;
    }
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (cauldronsData[i].cardsOnPile[j].color[0] != 'g' && cauldronsData[i].cardsOnPile[j].color[0] != 0)
            {
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                    piles[i].color[p] = cauldronsData[i].cardsOnPile[j].color[p];
            }
            piles[i].value += cauldronsData[i].cardsOnPile[j].value;
        }
    }
}

void addSmallestCardToPile (gameData myDeck, players playersStats[], cauldrons cauldronsData[], cardInfo piles[], cardInfo smallest[], int values[], int index[])
{
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (smallest[j].color[0] == piles[i].color[0] && smallest[j].color[2] == piles[i].color[2] && smallest[j].value != 0) {
                values[i] = smallest[j].value + piles[i].value;
                index[i] = j;
                smallest[j].value = 0;
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                    smallest[j].color[p] = 0;
            }
        }
    }
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            if (piles[i].value == 0 && smallest[j].value != 0) {
                values[i] = smallest[j].value + piles[i].value;
                index[i] = j;
                smallest[j].value = 0;
                for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                    smallest[j].color[p] = 0;
            }
        }
    }
}

void infoAboutCard (gameData myDeck, players playersStats[], cauldrons cauldronsData[], int values[], int index[], int &cardValue, char cardColor[], cardInfo smallestCopy[])
{
    int minId = -1;
    int temp = 100000;
    for (int i = 0; i < myDeck.cauldrons; i++)
    {
        if (temp > values[i] && values[i] != 0) {
            temp = values[i];
            minId = index[i];
        }
    }
    cardValue = smallestCopy[minId].value;
    for (int i = 0; i < MAX_LENGTH_OF_WORD_COLOR; i++)
        cardColor[i] = smallestCopy[minId].color[i];
}

void deleteCardFromHand (gameData myDeck, players playersStats[], int cardValue, char cardColor[])
{
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++)
    {
        if (playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i].value == cardValue && playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i].color[0] == cardColor[0] && playersStats[myDeck.activePlayer - 1].playerCardsOnHand[i].color[2] == cardColor[2])
        {
            for (int j = i; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND - 1; j++)
            {
                playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j] = playersStats[myDeck.activePlayer - 1].playerCardsOnHand[j + 1];
            }
            playersStats[myDeck.activePlayer - 1].playerCardsOnHand[MAX_CARDS_ON_PLAYER_DECK_OR_HAND - 1].value = 0;
            for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                playersStats[myDeck.activePlayer - 1].playerCardsOnHand[MAX_CARDS_ON_PLAYER_DECK_OR_HAND - 1].color[p] = 0;
            break;
        }
    }
}

void smallestCardMove (gameData &myDeck, players playersStats[], cauldrons cauldronsData[])
{
    int cardValue, pileId = -1;
    char cardColor[MAX_LENGTH_OF_WORD_COLOR];
    cardInfo *piles = new cardInfo[myDeck.cauldrons];
    cardInfo smallest[MAX_CARDS_ON_PLAYER_DECK_OR_HAND], smallestCopy[MAX_CARDS_ON_PLAYER_DECK_OR_HAND];
    int *values = new int[myDeck.cauldrons];
    int *index = new int[myDeck.cauldrons];
    for (int i = 0; i < myDeck.cauldrons; i++) {
        values[i] = 0;
        index[i] = 0;
    }
    for (int i = 0; i < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; i++) {
        smallest[i].value = 0;
        for (int j = 0; j < MAX_LENGTH_OF_WORD_COLOR; j++)
            smallest[i].color[j] = 0;
    }
    whichCards (myDeck, playersStats, cauldronsData, smallest, smallestCopy);
    whichPile (myDeck, playersStats, cauldronsData, piles);
    addSmallestCardToPile (myDeck, playersStats, cauldronsData, piles, smallest, values, index);
    infoAboutCard (myDeck, playersStats, cauldronsData, values, index, cardValue, cardColor, smallestCopy);
    deleteCardFromHand (myDeck, playersStats, cardValue, cardColor);
    addToPile(myDeck, cauldronsData, cardValue, cardColor);
    cauldronExplosion(myDeck, cauldronsData, playersStats, pileId);
    if (pileId != -1)
        addCardsAfterBoom(myDeck, cauldronsData, playersStats, pileId);
    if (myDeck.activePlayer == myDeck.players)
        myDeck.activePlayer = 1;
    else
        myDeck.activePlayer++;
//    printEasyMove(myDeck, cauldronsData, playersStats);
    delete[] piles;
    delete[] values;
    delete[] index;
}

int main()
{
    srand(time(NULL));
//    Wywolanie prostej rozgrywki dla wielu graczy
    gameData myDeck{};
    myDeck.activePlayer = 1;
    insertDeckData(myDeck);
    int deckSize = myDeck.greenCards + (myDeck.cauldrons * myDeck.otherCardValue);
    int whichPlayer = deckSize % myDeck.players;
    if (whichPlayer == 0)
        whichPlayer = myDeck.players - 1;
    cardInfo *generatedDeck = new cardInfo[deckSize];
    deckGeneration(myDeck, generatedDeck);
    players *playersStats = new players[myDeck.players];
    for (int i = 0; i < myDeck.players; i++)
    {
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            playersStats[i].playerCardsOnDeck[j].value = 0;
            playersStats[i].playerCardsOnHand[j].value = 0;
            for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
            {
                playersStats[i].playerCardsOnDeck[j].color[p] = 0;
                playersStats[i].playerCardsOnHand[j].color[p] = 0;
            }
        }
    }
    shuffleDeck(deckSize, generatedDeck);
    cauldrons *cauldronsData = new cauldrons[myDeck.cauldrons];
    for (int i = 0; i < myDeck.cauldrons; i++)
        for (int j = 0; j < MAX_CARDS_ON_PLAYER_DECK_OR_HAND; j++)
        {
            cauldronsData[i].cardsOnPile[j].value = 0;
            for (int p = 0; p < MAX_LENGTH_OF_WORD_COLOR; p++)
                cauldronsData[i].cardsOnPile[j].color[p] = 0;
        }
    int myNumber;
    std::cout << "Podaj numer gracza: " << std::endl;
    std::cin >> myNumber;
    if (myNumber == 1) {
        cardsDeal(playersStats, generatedDeck, myDeck, deckSize);
        generateGameDataFile(myDeck, cauldronsData, playersStats);
        std::cout << "Runda: 0" << std::endl;
    }
    else {
        loadGameFromFile(myDeck, playersStats, cauldronsData);
    }
    int iterator = 1;

    while (true)
    {
        if (playersStats[whichPlayer -1].playerCardsOnHand[0].value != 0) {
            loadGameFromFile(myDeck, playersStats, cauldronsData);
            if (myDeck.activePlayer == myNumber || myNumber == -1) {
                std::cout << "Runda: " << iterator << std::endl;
                if (isGameOk(myDeck, cauldronsData, playersStats) && myNumber == 2) {
                    easyMove(myDeck, cauldronsData, playersStats);
                    std::cout << "My turn is done..." << std::endl;
                }
                else if (isGameOk(myDeck, cauldronsData, playersStats) && myNumber == 1) {
                    smallestCardMove(myDeck, playersStats, cauldronsData);
                    std::cout << "My turn is done..." << std::endl;
                } else
                    break;

                std::cout << std::endl;
                iterator++;
                generateGameDataFile(myDeck, cauldronsData, playersStats);
            }
            Sleep(2000);
        }
        else
            break;
    }
    endOfRound(myDeck, cauldronsData, playersStats);
    getchar();
    getchar();

    delete[] cauldronsData;
    delete[] generatedDeck;
    delete[] playersStats;
}