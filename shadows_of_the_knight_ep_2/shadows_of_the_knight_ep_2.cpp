#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#define IS_DEBUG

#pragma region("COMMON TYPES")
using String = std::string;

template <typename T>
using Holder = std::unique_ptr<T>;

template <typename T, typename... Args>
Holder<T> MakeHolder(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
#pragma endregion

#pragma region("INPUT UTILS")
template <typename T>
T Read(std::istream& input) {
    T result;
    input >> result;
    return result;
}

template <typename T, typename PredicateType>
T CheckArgument(const T& argument, const PredicateType& pred, const String& message = "") {
    if (!pred(argument)) {
        std::stringstream formattedMessage;
        if (!message.empty()) {
            formattedMessage << message << ": ";
        }
        formattedMessage << "argument '" << argument << "' is incorrect";
        throw std::runtime_error(formattedMessage.str());
    }
    return argument;
}
#pragma endregion

#pragma region("MATH UTILS")
struct Point {
    int X = 0;
    int Y = 0;
};
#pragma endregion

#pragma region("GAME ENTITIES")
class Building final {
public:
    Building(int width, int height) noexcept
        : Width(CheckArgument(width, [](int w) {
            return w >= MIN_WIDTH && w <= MAX_WIDTH;
        }, "Building width"))
        , Height(CheckArgument(height, [](int h) {
            return h >= MIN_HEIGHT && h <= MAX_HEIGHT;
        }, "Building height"))
    {
    }

    int GetWidth() const {
        return Width;
    }

    int GetHeight() const {
        return Height;
    }

public:
    static constexpr int MIN_WIDTH = 1;
    static constexpr int MAX_WIDTH = 10000;
    static constexpr int MIN_HEIGHT = 5;
    static constexpr int MAX_HEIGHT = 10000;

private:
    const int Width;
    const int Height;
};

class Batman final {
public:
    Batman(int x, int y) noexcept
        : Position{
            CheckArgument(x, [](int x0) {
                return x0 >= 0 && x0 < Building::MAX_WIDTH;
            }, "Batman x0")
            , CheckArgument(y, [](int y0) {
                return y0 >= 0 && y0 < Building::MAX_HEIGHT;
            }, "Batman y0")
        }
    {
    }

    const Point& GetPosition() const {
        return Position;
    }

private:
    Point Position;
};
#pragma endregion

#pragma region("STRATEGY")
class IStrategy {
public:
    virtual ~IStrategy() = default;

    virtual Point MakeDecision(const String& bomdDirection) = 0;
};

class SimpleStrategy final : public IStrategy {
public:
    Point MakeDecision(const String& /*bombDirection*/) override {
        return {0, 0};
    }
};

Holder<IStrategy> CreateStrategy() {
    return MakeHolder<SimpleStrategy>();
}
#pragma endregion

class Game final {
public:
    explicit Game(std::istream& input) noexcept
        : House(ReadBuilding(input))
        , TurnsLeft(ReadTurns(input))
        , Player(ReadBatman(input))
        , Strategy(CreateStrategy())
    {
    }

    bool IsRunning() const {
        return TurnsLeft >= STOP_BELOW_TURNS;
    }

    void NextTurn(std::istream& input, std::ostream& output) {
        BeforeTurn();
        OnTurn(input, output);
        AfterTurn();
    }

private:
    static constexpr int STOP_BELOW_TURNS = 1;
    static constexpr int MIN_INPUT_TURNS = 2;
    static constexpr int MAX_INPUT_TURNS = 100;

    Building House;
    int TurnsLeft;
    Batman Player;
    Holder<IStrategy> Strategy;

private:
    static Building ReadBuilding(std::istream& input) {
        int w = Read<int>(input);
        int h = Read<int>(input);
        return {w, h};
    }

    static int ReadTurns(std::istream& input) {
        int turnsLeft = Read<int>(input);
        return CheckArgument(turnsLeft, [](int turns) {
            return turns >= MIN_INPUT_TURNS && turns <= MAX_INPUT_TURNS;
        }, "Turns left");
    }

    static Batman ReadBatman(std::istream& input) {
        int x = Read<int>(input);
        int y = Read<int>(input);
        return {x, y};
    }

    void BeforeTurn() {
        if (!IsRunning()) {
            throw std::runtime_error("The game is not running: no turns left");
        }
    }

    void OnTurn(std::istream& input, std::ostream& output) {
        String bombDir;
        input >> bombDir;
        const auto jumpTo = Strategy->MakeDecision(bombDir);
        output << jumpTo.X << " " << jumpTo.Y << std::endl;
    }

    void AfterTurn() {
        --TurnsLeft;
    }
};

int main(int argc, const char** argv)
{
    Game game(std::cin);
    while (game.IsRunning()) {
        game.NextTurn(std::cin, std::cout);
    }

    return 0;
}
