#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <ctime>
#include <algorithm>  
#include<queue>
#include<list>
#include <windows.h> 
using namespace std;
// colors
const string BLACK = "\033[30m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string WHITE = "\033[37m";
const string RESET = "\033[0m";

const string BRIGHT_BLACK = "\033[90m";
const string BRIGHT_RED = "\033[91m";
const string BRIGHT_GREEN = "\033[92m";
const string BRIGHT_YELLOW = "\033[93m";
const string BRIGHT_BLUE = "\033[94m";
const string BRIGHT_MAGENTA = "\033[95m";
const string BRIGHT_CYAN = "\033[96m";
const string BRIGHT_WHITE = "\033[97m";

//-------------------Map Elements-------------------//
const char WALL = '#';
const char PLAYER = 'P';
const char ZOMBIE = 'Z';
const char ITEM = '*';
const char SAFE_ZONE = 'S';
const char EMPTY = ' ';

const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 10;
const int INITIAL_HEALTH = 100;
const int ZOMBIE_DAMAGE = 25;
const int MAX_ZOMBIES = 5;
const int ITEM_SPAWN_RATE = 15;

//---------------------------Ascii Art----------------------------//
const string GAME_WON = R"(
                        ____ ___  _   _  ____ ____      _  _____ ____  
                       / ___/ _ \| \ | |/ ___|  _ \    / \|_   _/ ___| 
                      | |  | | | |  \| | |  _| |_) |  / _ \ | | \___ \ 
                      | |__| |_| | |\  | |_| |  _ <  / ___ \| |  ___) |
                       \____\___/|_| \_|\____|_| \_\/_/   \_\_| |____/ 
)";
const string WON = R"(
                      __   _____  _   ___        _____  _   _ 
                      \ \ / / _ \| | | \ \      / / _ \| \ | |
                       \ V / | | | | | |\ \ /\ / / | | |  \| |
                        | || |_| | |_| | \ V  V /| |_| | |\  |
                        |_| \___/ \___/   \_/\_/  \___/|_| \_|
)";
const string GAME_OVER = "GAME OVER!!";

const string EXIT = R"(
                        _______  _____ _____ ___ _   _  ____ 
                       | ____\ \/ /_ _|_   _|_ _| \ | |/ ___|        
                       |  _|  \  / | |  | |  | ||  \| | |  _         
                       | |___ /  \ | |  | |  | || |\  | |_| |_ _ _ _ 
                       |_____/_/\_\___| |_| |___|_| \_|\____(_|_|_|_)
)";

//----------------------Zombie Movement Elements------------------//
const int dx[4] = { -1, 1, 0, 0 }; // up, down, left, right
const int dy[4] = { 0, 0, -1, 1 };


//---------------------------Items Class--------------------------//
class Items {
public:
    string name;
    int value;
    int effect;
    Items* nextItem;

    // Parameterized constructor
    Items(string n, int v, int e) {
        name = n;
        value = v;
        effect = e;
        nextItem = NULL;
    }
    
};

//------------------------itemsList initializing------------------//
list<Items*> itemList;

void initializeItems() {
    string itemNames[30] = {
    "Band",   // Bandage
    "Medkit", // Medical Kit
    "PainX",  // Painkiller
    "Antib",  // Antibiotic
    "Syrup",  // Cough Syrup
    "Inhal",  // Inhaler
    "Spray",  // Disinfectant Spray
    "Gauze",  // Gauze Pad
    "Oint",   // Ointment
    "IVBag",  // IV Fluid Bag
    "Mask",   // Face Mask
    "Vitam",  // Vitamins
    "Drop",   // Eye Drops
    "Patch",  // Heat Patch
    "Gel",    // Burn Gel
    "Saline", // Saline Bottle
    "EpiPen", // Epinephrine Pen
    "Glove",  // Medical Gloves
    "Thermo", // Thermometer
    "Pads",   // Alcohol Pads
    "Insul",  // Insulin
    "Tabs",   // Tablets
    "Tonic",  // Energy Tonic
    "Shot",   // Injection Shot
    "Cream",  // Healing Cream
    "Stitch", // Stitch Kit
    "Wrap",   // Wrist Wrap
    "Razor",  // Razor Blade
    "Soap",   // Antibacterial Soap
    "Tape"    // Medical Tape
    };



    srand(time(0)); // Seed random number generator

    for (int i = 0; i < 30; i++) {
        int value = rand() % 10 + 50;
        int effect = rand() % 10;
        int r = rand() % 30;
        itemList.push_back(new Items(itemNames[r], value, effect));
    }
}



//---------------------------Player Class-------------------------//
class Player
{
public:
    int x_cor, y_cor;
    int health, score;
    Items* inventory;
    int ZombieEncounter;
    Player(int x = 0, int y = 0, int h = INITIAL_HEALTH)
    {
        x_cor = x;
        y_cor = y;
        health = h;
        score = 0;
        inventory = NULL;
        ZombieEncounter = 0;
    }
};

//-------------------------Zombie Class--------------------------//
class Zombie
{
public:
    int x, y;
};

//---------------------------Node Class--------------------------//
class Node
{
public:
    int x, y;
    char type;
    vector<pair<int, int>> neighbors;
    Node(int x = 0, int y = 0, char type = EMPTY)
    {
        this->x = x;
        this->y = y;
        this->type = type;
    }
};

//--------------------------Main Game Class----------------------//
class ZombieGame
{
private:
    // Graph of Nodes and Player
    vector<vector<Node>> graph;
    Player player;
    Items* inventory;
    int inventorySize;

public:
    ZombieGame()
    {
        srand(static_cast<unsigned int>(time(0)));
        initializeItems();
        initializeGraph();
        inventory = NULL;
        inventorySize = 0;
    }

    //-------------------Displaying Game function---------------//
    void gameFunction()
    {

        cout << "\n\n\n\n";
        cout << BRIGHT_CYAN << "                         =========================================================================\n";
        cout << BRIGHT_CYAN << "                         |" << BRIGHT_BLACK << "   [1] Manual Mode    " << BRIGHT_CYAN << "|" << BRIGHT_BLACK << "     [2] Automated Mode     " << BRIGHT_CYAN << "|" << BRIGHT_BLACK << "     [3] Exit      " << BRIGHT_CYAN << "|\n";
        cout << BRIGHT_CYAN << "                         =========================================================================\n";
        cout << BRIGHT_CYAN << "                         |" << BRIGHT_BLACK << "    Health:" << player.health << "    " << BRIGHT_CYAN << "|" << BRIGHT_BLACK << "    Score:" << player.score << "    " << BRIGHT_CYAN << "|" << BRIGHT_BLACK << "    Inventory:[";

        // Display inventory items
        Items* currentItem = inventory;
        while (currentItem != NULL)
        {
            cout << currentItem->name;
            if (currentItem->nextItem != NULL)
            {
                cout << ", ";
            }
            currentItem = currentItem->nextItem;
        }

        cout << BRIGHT_BLACK << "]" << BRIGHT_CYAN << "       |\n";
        cout << BRIGHT_CYAN << "                         =========================================================================\n";

        printGraph();

        cout << BRIGHT_CYAN << "                         =========================================================================\n";
        cout << BRIGHT_CYAN << "                         | Legend:                                                               |\n";
        cout << BRIGHT_CYAN << "                         |" << BRIGHT_BLACK << "      P = Player   Z = Zombie   S = Safe Zone   * = Item   # = Wall    " << BRIGHT_CYAN << "|\n";
        cout << BRIGHT_CYAN << "                         =========================================================================\n";
        cout << RESET;

        //winning condition check
        if (player.x_cor == MAP_HEIGHT - 2 && player.y_cor == MAP_WIDTH - 2)
        {
            /* graph[player.x_cor][player.y_cor].type = EMPTY;
             player.x_cor = MAP_HEIGHT - 2;
             player.y_cor = MAP_WIDTH - 2;
             graph[player.x_cor][player.y_cor].type = PLAYER;*/

            clearing();


            cout << "\n\n\n\n\n\n\n" << BRIGHT_BLUE << GAME_WON
                << "\n" << WON;
            exit(0);

        }
        //Zombie reaches player check





    }
    //---------------------Inititalizing Graph------------------//
    void initializeGraph()
    {
        // Initialize the graph with empty spaces
        graph.resize(MAP_HEIGHT, vector<Node>(MAP_WIDTH));

        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            for (int j = 0; j < MAP_WIDTH; j++)
            {
                graph[i][j] = Node(i, j, EMPTY);
            }
        }

        // Adding the border walls along Columns(Height)
        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            graph[i][0].type = WALL;
            graph[i][MAP_WIDTH - 1].type = WALL;
        }
        // Adding the border walls along Rows(Width)
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            graph[0][j].type = WALL;
            graph[MAP_HEIGHT - 1][j].type = WALL;
        }

        // Adding inner walls at 5th,10th and 15th column
        for (int i = 2; i < MAP_HEIGHT - 2; i++)
        {
            graph[i][5].type = WALL;
            graph[i][10].type = WALL;
            graph[i][15].type = WALL;
        }

        // Set neighbors (graph edges)
        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            for (int j = 0; j < MAP_WIDTH; j++)
            {
                if (graph[i][j].type != WALL)
                {
                    if (i > 0 && graph[i - 1][j].type != WALL)
                    {
                        graph[i][j].neighbors.push_back({ i - 1, j });
                    }
                    if (i < MAP_HEIGHT - 1 && graph[i + 1][j].type != WALL)
                    {
                        graph[i][j].neighbors.push_back({ i + 1, j });
                    }
                    if (j > 0 && graph[i][j - 1].type != WALL)
                    {
                        graph[i][j].neighbors.push_back({ i, j - 1 });
                    }
                    if (j < MAP_WIDTH - 1 && graph[i][j + 1].type != WALL)
                    {
                        graph[i][j].neighbors.push_back({ i, j + 1 });
                    }
                }
            }
        }

        // Setting the player at 1,1
        player = Player(1, 1);
        graph[1][1].type = PLAYER;

        // Setting safe zone
        graph[MAP_HEIGHT - 2][MAP_WIDTH - 2].type = SAFE_ZONE;

        // Spawn items
        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            for (int j = 0; j < MAP_WIDTH; j++)
            {
                if (graph[i][j].type == EMPTY)
                {
                    int random = rand() % 100;
                    if (random < ITEM_SPAWN_RATE)
                    {
                        graph[i][j].type = ITEM;

                    }
                }
            }
        }
        vector<pair<int, int>> emptyCells;

        // Collect empty positions
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                if (graph[i][j].type == EMPTY)
                    emptyCells.push_back({ i, j });
            }
        }


        srand(time(0));
        random_shuffle(emptyCells.begin(), emptyCells.end());

        // Spawning zombies
        int zombieCount = 0;
        for (auto& pos : emptyCells) {
            if (zombieCount >= MAX_ZOMBIES) {
                break;
            }

            graph[pos.first][pos.second].type = ZOMBIE;
            zombieCount++;
        }
    }
    //---------------------  Printing Map-----------------------//
    void printGraph()
    {
        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            cout << "                          ";
            for (int j = 0; j < MAP_WIDTH; j++)
            {
                char cell = graph[i][j].type;
                switch (cell)
                {
                case ZOMBIE:
                    cout << BRIGHT_RED;
                    break;
                case PLAYER:
                    cout << BRIGHT_BLUE;
                    break;
                case WALL:
                    cout << BRIGHT_GREEN;
                    break;
                case ITEM:
                    cout << BRIGHT_BLACK;
                    break;
                case SAFE_ZONE:
                    cout << BRIGHT_WHITE;
                    break;

                default:
                    cout << RESET;
                }

                cout << cell << " ";
            }
            if (i == 3)
            {
                cout << BRIGHT_CYAN << " ===============================";
            }
            if (i == 4)
            {
                cout << BRIGHT_CYAN << " =" << BRIGHT_YELLOW << "     ZOMBIE" << BRIGHT_BLUE << " APOCALYPSE      " << BRIGHT_CYAN << " = ";
            }
            if (i == 5)
            {
                cout << BRIGHT_CYAN << " =" << WHITE << "   1)Sammad Israr->23i-3042" << BRIGHT_CYAN << "  = ";
            }
            if (i == 6)
            {
                cout << BRIGHT_CYAN << " =" << WHITE << "   2)Abdul  Moeed->23i-3101" << BRIGHT_CYAN << "  =";
            }if (i == 7)
            {
                cout << BRIGHT_CYAN << " ===============================";
            }
            cout << RESET << endl;
        }
    }
    //-----------------Adding Item To Inventory-----------------//
    void addItemToInventory(Items* item) {
        if (item == NULL) {
            return;
        }
        if (inventorySize > 5)
        {
            cout << "                         ";
            cout << RED << "Max Limit : 5 Items\n";
            return;
        }

        Items* newItem = new Items(item->name, item->value, item->effect);
        newItem->nextItem = NULL;

        if (inventory == NULL) {
            inventory = newItem;
        }
        else {
            Items* temp = inventory;
            while (temp->nextItem != NULL) {
                temp = temp->nextItem;
            }
            temp->nextItem = newItem;
        }
        inventorySize++;

    }
    //--------------------Helper Movement Function- ------------//
    void movePlayer(string key)
    {
        if (key == "w")
        {
            moveUp();
        }
        else if (key == "s")
        {
            moveDown();
        }
        else if (key == "d")
        {
            moveRight();
        }
        else if (key == "a")
        {
            moveLeft();
        }
    }
    //------------------Movement Controlling functions----------//
    void moveUp() {
        int newX = player.x_cor - 1;
        int newY = player.y_cor;

        if (newX >= 0 && graph[newX][newY].type != WALL) {
            if (graph[newX][newY].type == ZOMBIE) {
                cout << "                           " << BRIGHT_RED << "Zombie Attacked!" << endl;
                healthDeduction();
                calScore();

            }
            else if (graph[newX][newY].type == ITEM) {
                cout << "                         " << YELLOW << "Item Collected!\n" << RESET;
                calScore();
                if (!itemList.empty()) {
                    addItemToInventory(itemList.front());
                    itemList.pop_front();
                }
                else {
                    cout << RED << "No items left in the world!\n" << RESET;
                }
            }

            graph[player.x_cor][player.y_cor].type = EMPTY;
            player.x_cor = newX;
            player.y_cor = newY;
            graph[newX][newY].type = PLAYER;
        }
    }
    void moveDown() {
        int newX = player.x_cor + 1;
        int newY = player.y_cor;

        if (newX < MAP_HEIGHT && graph[newX][newY].type != WALL) {
            if (graph[newX][newY].type == ZOMBIE) {
                cout << "                           " << BRIGHT_RED << "Zombie Attacked!" << endl;
                healthDeduction();
                calScore();

            }
            else if (graph[newX][newY].type == ITEM) {
                cout << "                         " << YELLOW << "Item Collected!\n" << RESET;
                calScore();

                if (!itemList.empty()) {
                    addItemToInventory(itemList.front());
                    itemList.pop_front();
                }
                else {
                    cout << RED << "No items left in the world!\n" << RESET;
                }
            }

            graph[player.x_cor][player.y_cor].type = EMPTY;
            player.x_cor = newX;
            player.y_cor = newY;
            graph[newX][newY].type = PLAYER;
        }
    }
    void moveLeft() {
        int newX = player.x_cor;
        int newY = player.y_cor - 1;

        if (newY >= 0 && graph[newX][newY].type != WALL) {
            if (graph[newX][newY].type == ZOMBIE) {
                cout << "                           " << BRIGHT_RED << "Zombie Attacked!" << endl;
                healthDeduction();
                calScore();

            }
            else if (graph[newX][newY].type == ITEM) {
                cout << "                         " << YELLOW << "Item Collected!\n" << RESET;
                calScore();

                if (!itemList.empty()) {
                    addItemToInventory(itemList.front());
                    itemList.pop_front();
                }
                else {
                    cout << RED << "No items left in the world!\n" << RESET;
                }
            }

            graph[player.x_cor][player.y_cor].type = EMPTY;
            player.x_cor = newX;
            player.y_cor = newY;
            graph[newX][newY].type = PLAYER;
        }
    }
    void moveRight() {
        int newX = player.x_cor;
        int newY = player.y_cor + 1;

        if (newY < MAP_WIDTH && graph[newX][newY].type != WALL) {
            if (graph[newX][newY].type == ZOMBIE) {
                cout << "                           " << BRIGHT_RED << "Zombie Attacked!" << endl;
                healthDeduction();
                calScore();

            }
            else if (graph[newX][newY].type == ITEM) {
                cout << "                         " << YELLOW << "Item Collected!\n" << RESET;
                calScore();

                if (!itemList.empty()) {
                    addItemToInventory(itemList.front());
                    itemList.pop_front();
                }
                else {
                    cout << RED << "No items left in the world!\n" << RESET;
                }
            }

            graph[player.x_cor][player.y_cor].type = EMPTY;
            player.x_cor = newX;
            player.y_cor = newY;
            graph[newX][newY].type = PLAYER;
        }
    }

    //---------------------Manual Mode-------------------------//
    void manualMode() {
        if (player.health<=0)
        {
            cout << "                               " << BRIGHT_RED << "GAME OVER!\n";
            return;
        }
        string inp;
        cout << "                         ";
        //Use Item check
        if (inventorySize > 0)
        {
            cout << "Input(w,a,s,d & Y to use items) :- ";
            cin >> inp;
            if (inp == "Y")
            {
                useItem();
            }

        }
        
        //Only Movements
        else {
            cout << "Movement (w,a,s,d) :- ";
            cin >> inp;

        }
        


        movePlayer(inp);
        gameFunction();
        moveZombiesTowardsPlayer(player.x_cor, player.y_cor);
        manualMode();
    }
    //---------------------Manual Mode-------------------------//
    void autoMode() {

    }
    //-------------------Taking inputs-------------------------//
    void takeInput() {
        gameFunction();
        cout << "                         ";
        cout << "Mode Selection:- ";
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 1:
            manualMode();
            break;

        case 2:
            autoMode();
            break;
        case 3:
            clearing();
            cout << BRIGHT_BLUE << "\n\n\n\n                         ";
            cout << EXIT;
            exit(0);
            break;
        default:
            while (choice < 1 || choice > 3) {
                cout << "                         ";
                cout << BRIGHT_RED << "Invalid input!!\nEnter again:- ";
                cout << "                         ";
                cin >> choice;
            }
            break;
        }
    }
    //-------------------Screen Clearing-----------------------//
    void clearing() {
#ifdef _WIN32
        system("cls");  // Windows
#else
        system("clear"); // Unix/Linux/Mac
#endif
    }
    //-------------------Score Calculation---------------------//
    void calScore() {
        //score calculation
        if (inventory!=NULL)
        {
        player.score += (10*inventory->effect) - (player.ZombieEncounter*5) + (inventorySize/5);

        }
        else {
            player.score += ((inventorySize/2) - (player.ZombieEncounter * 5)) ;

        }
    }
    //-------------------Health Deduction----------------------//
    void healthDeduction() {
        player.health -= 23;
       
    }
    //--------------------Use Item-----------------------------//
    void useItem() {
        if (player.health == 100) {
            cout << "                          " << BRIGHT_GREEN << "Can't Use Item [HEALTH==100]\n";
            return;
        }
        player.health += (inventory->value/3);
        if (player.health > 100)
        {
            player.health = 100;
        }
        Items* temp = inventory;
        inventory = inventory->nextItem;
        delete temp;
        inventorySize--;

    }
    //--------------------Move Zombies-------------------------//
    void moveZombiesTowardsPlayer(int playerX, int playerY) {
        vector<vector<bool>> visited(MAP_HEIGHT, vector<bool>(MAP_WIDTH, false));
        vector<vector<pair<int, int>>> parent(MAP_HEIGHT, vector<pair<int, int>>(MAP_WIDTH, { -1, -1 }));

        queue<pair<int, int>> q;
        q.push({ playerX, playerY });
        visited[playerX][playerY] = true;

        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };

        // Step 1: BFS from the player
        while (!q.empty()) {
            pair<int, int> front = q.front(); q.pop();
            int x = front.first;
            int y = front.second;

            for (int d = 0; d < 4; ++d) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                if (nx >= 0 && nx < MAP_HEIGHT && ny >= 0 && ny < MAP_WIDTH &&
                    !visited[nx][ny] && graph[nx][ny].type != WALL) {
                    visited[nx][ny] = true;
                    parent[nx][ny] = { x, y };
                    q.push({ nx, ny });
                }
            }
        }

        // Step 2: Move zombies
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                if (graph[i][j].type == ZOMBIE) {
                    int zx = i, zy = j;
                    if (!visited[zx][zy]) continue;

                    // Backtrack one step toward player
                    pair<int, int> curr = { zx, zy };
                    while (parent[curr.first][curr.second] != make_pair(playerX, playerY) &&
                        parent[curr.first][curr.second] != make_pair(-1, -1)) {
                        curr = parent[curr.first][curr.second];
                    }

                    int nx = curr.first;
                    int ny = curr.second;

                    // Move zombie
                    if (graph[nx][ny].type == EMPTY || graph[nx][ny].type == PLAYER) {
                        if (graph[nx][ny].type == PLAYER) {
                            cout << "                   " << BRIGHT_RED << "Zombie Attacked!\n" << RESET;
                            player.ZombieEncounter++;
                            healthDeduction();
                        }

                        graph[zx][zy].type = EMPTY;
                        graph[nx][ny].type = ZOMBIE;
                    }
                }
            }
        }
    }

};


       


int main() {
    // Enable ANSI escape codes in Windows Command Prompt
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    ZombieGame g;
    g.takeInput();

    return 0;
}