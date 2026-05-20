#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <windows.h>
#include <vector>

using namespace std;

#define RESET      "\033[0m"
#define BOLD       "\033[1m"
#define FG_CYAN    "\033[36m"
#define FG_WHITE   "\033[97m"
#define FG_RED     "\033[31m"
#define FG_GREEN   "\033[32m"
#define FG_YELLOW  "\033[33m"

void clearScreen()
{
    Sleep(400);
    cout << "\033[2J\033[H";
    cout.flush();
}

void shuffleAnimation(char finalMove)
{
    char moves[3] = { 'R', 'P', 'S' };
    cout << endl;
    for (int i = 0; i < 15; i++)
    {
        cout << "\r  " FG_CYAN BOLD "Computer choosing: "
            << moves[i % 3] << "   " RESET;
        cout.flush();
        Sleep(130);
    }
    cout << "\r  " FG_CYAN BOLD "Computer choosing: " << finalMove << "   " RESET;
    cout.flush();
    Sleep(400);
    cout << endl;
}

void flashResult(const string& text, const char* color)
{
    for (int i = 0; i < 5; i++)
    {
        cout << "\r  " << color << BOLD << text << RESET << "            ";
        cout.flush();
        Sleep(280);
        cout << "\r                                                  ";
        cout.flush();
        Sleep(280);
    }
    cout << "\r  " << color << BOLD << text << RESET << endl;
}

void printLine(const string& text)
{
    cout << FG_CYAN BOLD "   " << text << RESET << endl;
}

// ---------------------------------------------------------------
// visWidth: counts visible terminal columns for a UTF-8 string,
// correctly handling multi-byte sequences and variation selectors.
// This is what printBox uses to pad lines, so boxes stay square.
// ---------------------------------------------------------------
int visWidth(const string& s)
{
    int w = 0;
    for (int i = 0; i < (int)s.size(); )
    {
        unsigned char c = (unsigned char)s[i];
        unsigned int  cp = 0;
        int           bytes = 1;

        if (c < 0x80) { bytes = 1; cp = c; }
        else if ((c & 0xE0) == 0xC0) { bytes = 2; cp = c & 0x1F; }
        else if ((c & 0xF0) == 0xE0) { bytes = 3; cp = c & 0x0F; }
        else if ((c & 0xF8) == 0xF0) { bytes = 4; cp = c & 0x07; }

        for (int j = 1; j < bytes && i + j < (int)s.size(); j++)
            cp = (cp << 6) | ((unsigned char)s[i + j] & 0x3F);

        i += bytes;

        // Zero-width: variation selectors (e.g. U+FE0F makes ✂ look like ✂️)
        // and ZWJ (U+200D used in compound emoji).
        if (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))
            continue;

        // Wide (2-column): supplementary-plane emoji (U+1F000+),
        // Miscellaneous Symbols, Dingbats, and a handful of other blocks.
        if (cp >= 0x1F000) { w += 2; continue; }
        if (cp >= 0x2600 && cp <= 0x27BF) { w += 2; continue; }
        if (cp >= 0x2B00 && cp <= 0x2BFF) { w += 2; continue; }
        if (cp >= 0x3000 && cp <= 0x9FFF) { w += 2; continue; }

        w += 1;
    }
    return w;
}

void printBox(const string& line1, const string& line2 = "")
{
    const int width = 42;

    string bar(width, '=');

    cout << FG_WHITE BOLD
        << "   +" << bar << "+"
        << RESET << endl;

    // Uses visWidth() so that emoji bytes don't shrink the padding.
    auto printBoxLine = [&](const string& text)
        {
            cout << FG_WHITE BOLD
                << "   |  "
                << RESET;

            cout << FG_CYAN BOLD
                << text
                << RESET;

            int spaces = width - visWidth(text);
            if (spaces < 0) spaces = 0;

            for (int i = 0; i < spaces; i++)
                cout << " ";

            cout << FG_WHITE BOLD
                << "  |"
                << RESET << endl;
        };

    printBoxLine(line1);

    if (!line2.empty())
        printBoxLine(line2);

    cout << FG_WHITE BOLD
        << "   +" << bar << "+"
        << RESET << endl;
}

void printLevelBanner(int level)
{
    cout << endl;
    if (level == 1)
        printBox("\xF0\x9F\xAA\xA8  LEVEL 1  ( EASY MODE )", "   Computer always picks Rock");
    else if (level == 2)
        printBox("\xF0\x9F\x94\xA5  LEVEL 2  ( MEDIUM MODE )", "   Computer picks randomly");
    else
        printBox("\xE2\x9A\xA1  LEVEL 3  ( HARD MODE )", "   Computer beats your last move");
    cout << endl;
}

void printRoundHeader(int round)
{
    cout << endl;
    printBox("\xE2\x9A\x94\xEF\xB8\x8F  ROUND " + to_string(round) + "  \xE2\x9A\x94\xEF\xB8\x8F");
    cout << endl;
}

// Returns a short emoji + label for a move character.
string moveLabel(char m)
{
    if (m == 'R') return "\xF0\x9F\xAA\xA8 Rock";
    if (m == 'P') return "\xF0\x9F\x93\x84 Paper";
    return "\xE2\x9C\x82\xEF\xB8\x8F Scissors";
}

class Player
{
protected:
    string name;
    int    score;

public:
    Player(string n) : name(n), score(0) {}

    string getName()  const { return name; }
    int    getScore() const { return score; }
    void   addPoint() { score++; }
    void   resetScore() { score = 0; }

    virtual char makeChoice() = 0;
    virtual ~Player() {}
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(string n) : Player(n) {}

    char makeChoice() override
    {
        char move;
        while (true)
        {
            cout << FG_CYAN BOLD "  Enter R / P / S: " RESET;
            cin >> move;
            move = toupper(move);

            if (move == 'R' || move == 'P' || move == 'S')
                return move;

            cout << FG_CYAN BOLD "  Invalid! Enter R (Rock), P (Paper), or S (Scissors)." RESET << endl;
        }
    }
};

class ComputerPlayer : public Player
{
public:
    ComputerPlayer(string n) : Player(n) {}
    virtual char makeChoice() = 0;
};

class EasyComputer : public ComputerPlayer
{
public:
    EasyComputer() : ComputerPlayer("Computer (Easy)") {}
    char makeChoice() override { return 'R'; }
};

class MediumComputer : public ComputerPlayer
{
public:
    MediumComputer() : ComputerPlayer("Computer (Medium)") {}

    char makeChoice() override
    {
        int r = rand() % 3;
        if (r == 0) return 'R';
        if (r == 1) return 'P';
        return 'S';
    }
};

class HardComputer : public ComputerPlayer
{
private:
    char lastPlayerMove;

public:
    HardComputer() : ComputerPlayer("Computer (Hard)"), lastPlayerMove('R') {}

    void setLastPlayerMove(char m) { lastPlayerMove = m; }
    void resetLastMove() { lastPlayerMove = 'R'; }

    char makeChoice() override
    {
        if (lastPlayerMove == 'R') return 'P';
        if (lastPlayerMove == 'P') return 'S';
        return 'R';
    }
};

class round_record
{
    char   player_move;
    char   comp_move;
    string winner;

public:
    round_record()
    {
        player_move = 'p';
        comp_move = 'c';
    }

    void set_player_move(char p) { player_move = p; }
    void set_comp_move(char c) { comp_move = c; }
    void set_winner(string w) { winner = w; }

    char   get_player_move() { return player_move; }
    char   get_comp_move() { return comp_move; }
    string get_winner() { return winner; }
};


class Game
{
private:
    Player* human;
    ComputerPlayer* computer;
    int             totalRounds;
    int             level;
    vector<round_record> history;

    int evaluate(char h, char c)
    {
        if (h == c) return 0;
        if ((h == 'R' && c == 'S') ||
            (h == 'P' && c == 'R') ||
            (h == 'S' && c == 'P'))
            return 1;
        return -1;
    }

    void printScoreboard()
    {
        cout << FG_CYAN BOLD;
        cout << "  \xF0\x9F\x91\xA4 " << human->getName() << ": " << human->getScore() << endl;
        cout << "  \xF0\x9F\xA4\x96 " << computer->getName() << ": " << computer->getScore() << endl;
        cout << RESET;
    }

public:
    Game(Player* h, ComputerPlayer* c, int rounds, int lvl)
        : human(h), computer(c), totalRounds(rounds), level(lvl) {
    }

    void add_to_history(char humanMove, char compMove, int result, int /*round*/)
    {
        round_record r;
        r.set_player_move(humanMove);
        r.set_comp_move(compMove);

        if (result == 0) r.set_winner("Tie");
        else if (result == 1) r.set_winner("Human (You)");
        else                   r.set_winner("Computer");

        history.push_back(r);
    }

    // -----------------------------------------------------------
    // display_history: shows every round with cyan labels, colour-
    // coded result lines and emojis, all inside the same boxes
    // used elsewhere in the game.
    // -----------------------------------------------------------
    void display_history()
    {
        cout << endl;
        printBox("\xF0\x9F\x93\x8B  ROUND HISTORY  \xF0\x9F\x93\x8B");
        cout << endl;

        // Cast to int to avoid signed/unsigned comparison warning.
        for (int i = 0; i < (int)history.size(); i++)
        {
            // Sub-header box for each round.
            printBox("\xE2\x9A\x94\xEF\xB8\x8F  ROUND " + to_string(i + 1) + "  \xE2\x9A\x94\xEF\xB8\x8F");
            cout << endl;

            // Player move — cyan.
            cout << FG_CYAN BOLD
                << "  \xF0\x9F\x91\xA4 Your Move    :  "
                << moveLabel(history[i].get_player_move())
                << RESET << endl;

            // Computer move — cyan.
            cout << FG_CYAN BOLD
                << "  \xF0\x9F\xA4\x96 Computer     :  "
                << moveLabel(history[i].get_comp_move())
                << RESET << endl;

            cout << endl;

            // Result — colour matches the win/loss/tie palette used
            // in flashResult() during the live round.
            string winner = history[i].get_winner();

            if (winner == "Tie")
            {
                cout << "  " << FG_YELLOW BOLD
                    << "\xF0\x9F\xA4\x9D  TIE! No points awarded.  \xF0\x9F\xA4\x9D"
                    << RESET << endl;
            }
            else if (winner == "Human (You)")
            {
                cout << "  " << FG_GREEN BOLD
                    << "\xF0\x9F\x8E\x89  YOU WON THIS ROUND!  \xF0\x9F\x8E\x89"
                    << RESET << endl;
            }
            else
            {
                cout << "  " << FG_RED BOLD
                    << "\xF0\x9F\x92\x80  COMPUTER WON THIS ROUND!  \xF0\x9F\x92\x80"
                    << RESET << endl;
            }

            cout << endl;
        }
    }

    void play()
    {
        printLevelBanner(level);

        for (int round = 1; round <= totalRounds; round++)
        {
            printRoundHeader(round);

            char humanMove = human->makeChoice();
            char compMove = computer->makeChoice();

            HardComputer* hard = dynamic_cast<HardComputer*>(computer);
            if (hard) hard->setLastPlayerMove(humanMove);

            shuffleAnimation(compMove);

            cout << FG_CYAN BOLD "  You chose     : " << humanMove << RESET << endl;
            cout << FG_CYAN BOLD "  \xF0\x9F\xA4\x96 Computer chose: " << compMove << RESET << endl;
            cout << endl;

            int result = evaluate(humanMove, compMove);

            if (result == 0)
                flashResult("\xF0\x9F\xA4\x9D  TIE! No points awarded.  \xF0\x9F\xA4\x9D", FG_YELLOW);
            else if (result == 1)
            {
                human->addPoint();
                flashResult("\xF0\x9F\x8E\x89  YOU WIN THIS ROUND!  \xF0\x9F\x8E\x89", FG_GREEN);
            }
            else
            {
                computer->addPoint();
                flashResult("\xF0\x9F\x92\x80  COMPUTER WINS THIS ROUND!  \xF0\x9F\x92\x80", FG_RED);
            }

            cout << endl;
            cout << FG_CYAN BOLD "  Score:" RESET << endl;
            printScoreboard();

            add_to_history(humanMove, compMove, result, round);
        }

        cout << endl;
        printBox("\xF0\x9F\x8F\x86  MATCH RESULT  \xF0\x9F\x8F\x86");
        cout << endl;
        printScoreboard();
        cout << endl;

        if (human->getScore() > computer->getScore())
            flashResult("\xF0\x9F\x8F\x86  YOU WON THE MATCH!  \xF0\x9F\x8F\x86", FG_GREEN);
        else if (computer->getScore() > human->getScore())
            flashResult("\xF0\x9F\x92\x80  COMPUTER WON THE MATCH!  \xF0\x9F\x92\x80", FG_RED);
        else
            flashResult("\xF0\x9F\xA4\x9D  MATCH DRAW!  \xF0\x9F\xA4\x9D", FG_YELLOW);

        cout << endl;
        display_history();
    }
};


int getLevelChoice()
{
    cout << endl;
    printLine("\xF0\x9F\x8E\xAE  ROCK  PAPER  SCISSORS  \xF0\x9F\x8E\xAE");
    cout << endl;
    printLine("\xE2\x98\x80\xEF\xB8\x8F  Choose Computer Level:");
    cout << endl;
    printLine("1.  \xF0\x9F\xAA\xA8  EASY   ");
    printLine("2.  \xF0\x9F\x93\x84  MEDIUM  ");
    printLine("3.  \xE2\x9A\xA1  HARD    ");
    cout << endl;

    int level;
    while (true)
    {
        cout << FG_CYAN BOLD "  Choose level (1 / 2 / 3): " RESET;

        if (cin >> level && (level >= 1 && level <= 3))
            return level;

        cin.clear();
        cin.ignore(1000, '\n');
        cout << FG_CYAN BOLD "  Invalid! Please enter 1, 2, or 3." RESET << endl;
    }
}

ComputerPlayer* createComputerPlayer(int level)
{
    if (level == 1) return new EasyComputer();
    if (level == 2) return new MediumComputer();
    return new HardComputer();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    srand(static_cast<unsigned>(time(nullptr)));

    cout << FG_CYAN BOLD "\n  \xF0\x9F\x8E\xAE  Enter your name: " RESET;

    string playerName;
    cin >> playerName;

    HumanPlayer human(playerName);

    while (true)
    {
        int level = getLevelChoice();

        ComputerPlayer* comp = createComputerPlayer(level);

        human.resetScore();
        comp->resetScore();

        HardComputer* hard = dynamic_cast<HardComputer*>(comp);
        if (hard) hard->resetLastMove();

        Game game(&human, comp, 3, level);
        game.play();

        char answer;
        while (true)
        {
            cout << FG_CYAN BOLD "  Play again? (Y / N): " RESET;
            cin >> answer;
            answer = toupper(answer);

            if (answer == 'Y' || answer == 'N') break;

            cout << FG_CYAN BOLD "  Invalid! Please enter Y or N." RESET << endl;
        }

        delete comp;

        if (answer == 'N')
        {
            cout << endl;
            printLine("\xF0\x9F\x91\x8B  Thanks for playing. Goodbye!  \xF0\x9F\x91\x8B");
            cout << endl;
            break;
        }

        clearScreen();
    }

    return 0;
}