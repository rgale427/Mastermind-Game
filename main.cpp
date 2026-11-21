#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <memory>
#include <random>
// Abstract Base Class
class BaseGame {
protected:
    std::string m_secretCode;
    int m_maxAttempts;
    int m_currentAttempts;
    int m_codeLength;
    std::string m_correct;

    // Functions that must be implemented by derived classes
    virtual void generateSecretCode() =0; //1 needs to be overridden
    virtual bool isValidGuess(const std::string& guess) const =0;//2 needs to be overridden
    virtual std::string getGameModeRules() const =0;//3 needs to be overridden

    // Inherited helper function to check for duplicate characters
    bool hasUniqueCharacters(const std::string& str) const {
        for (size_t i = 0; i < str.length(); ++i)
        {
            for (size_t j = i + 1; j < str.length(); ++j)
            {
                if (str[i] == str[j]) return false;

            }
        }
        return true;
    }

    // Inherited Core Red and White evaluation logic
    std::pair<int, int> calculateRedAndWhite(const std::string& guess) {
        int red = 0;
        int white = 0;

        for (int i = 0; i < m_codeLength; ++i) {
            if (guess[i] == m_secretCode[i]) {
                red++;
                m_correct[i] = m_secretCode[i];
            } else if (m_secretCode.find(guess[i]) != std::string::npos) {
                white++;
            }
        }
        return {red, white};
    }

public:
    BaseGame(int length, int attempts){
        m_codeLength = length;
        m_maxAttempts = attempts;
        m_currentAttempts = 0;
        m_correct = "xxxxx";
    }

    // Virtual destructor is essential for base classes with virtual functions
    virtual ~BaseGame() = default;

    // Inherited method for controlling the main game flow
    void play() {
        // call to generate a secret code
generateSecretCode();
        std::cout << "\n--- Starting Game ---\n";
        std::cout << getGameModeRules() << "\n";
        std::cout << "You have " << m_maxAttempts << " attempts.\n\n";

        while (m_currentAttempts < m_maxAttempts) {
            std::string guess;
            std::cout << "Attempt " << (m_currentAttempts + 1) << "/" << m_maxAttempts << " - Enter guess: ";
            std::cin >> guess;

            if (!isValidGuess(guess)) {
                std::cout << "Invalid input! Please follow the rules.\n";
                continue;
            }

            m_currentAttempts++;
            auto [red, white] = calculateRedAndWhite(guess); //crashes here

            if (red == m_codeLength) {
                std::cout << "\nCongratulations! You guessed the secret code: " << m_secretCode << "\n";
                return;
            }
            std::cout << "Correct ->" << m_correct << std::endl;
            std::cout << "Result: " << red << " Red(s), " << white << " White(s)\n\n";
        }

        std::cout << "\nGame Over! You ran out of attempts. The secret code was: " << m_secretCode << "\n";
    }
};

// Derived Class 1: 4-Digit Number Mode
class NumericGame:public BaseGame {

protected:
    void generateSecretCode()  override {
        std::string digits = "0123456789";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(digits.begin(), digits.end(), gen);
       m_secretCode = digits.substr(0, m_codeLength);
    }

    bool isValidGuess(const std::string& guess) const override {
        if (guess.length() != static_cast<size_t>(m_codeLength)) return false;
        for (char c : guess) {
            if (!std::isdigit(c)) return false;
        }
        return hasUniqueCharacters(guess);
    }

    std::string getGameModeRules() const override {
        return "Mode: Numbers. Guess a " + std::to_string(m_codeLength) + "-digit number with unique digits (0-9).";
    }

public:
    NumericGame(int length = 4, int attempts = 10): BaseGame(length, attempts) {
         m_correct = "xxxx";
    }
};

// Derived Class 2: Isogram Word Mode
class WordGame:public BaseGame  {
private:
    std::vector<std::string> wordBank = {"plane", "steam", "coast", "bride", "white", "false"};

protected:
    void generateSecretCode() override {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, wordBank.size() - 1);
        m_secretCode = wordBank[dist(gen)];
        m_codeLength = m_secretCode.length();
    }

    bool isValidGuess(const std::string& guess) const override {
        if (guess.length() != static_cast<size_t>(m_codeLength)) return false;
        for (char c : guess) {
            if (!std::islower(c)) return false;
        }
        return hasUniqueCharacters(guess);
    }

    std::string getGameModeRules() const override {
        return "Mode: Word Isogram. Guess a " + std::to_string(m_codeLength) + "-letter lowercase word with no repeating letters.";
    }

public:
    WordGame(int attempts = 10): BaseGame(5, attempts) {
        m_correct = "xxxxx";
    }
};

int main() {
    std::unique_ptr<BaseGame> game;
    int choice;
    std::cout << "Select Game Mode:\n1. Number Mode (4 digits)\n2. Word Mode (6 letters)\nChoice: ";
std::cin >> choice;

    // Create a polymorphic pointer that can point to a BaseGame object or any class of object derived from BaseGame
    // Currently hold nullptr (nothing)
    // unique_ptr means the object will automatically be deleted when it goes out of scope to prevent memory leak


  if (choice == 2) {
        // Instantiate a new WordGame object on heap memory passing 10 to the constructor
        // Then assign assign ownership of the object to the game pointer
     game = std::make_unique<WordGame>(10);
    } else {
        // Instantiate a new NumericGame object on heap memory passing 4 and 10 to the constructor
        // Then assign assign ownership of the object to the game pointer
  game = std::make_unique<NumericGame>(4, 6);
    }

    /*
     * Unified Interface: Even though WordGame and NumericGame are different classes, storing them in a
     * BaseGame pointer lets the rest of your program interact with game the exact same way
     * without needing to know which specific version was created.
     */
game -> play();
    return 0;
}
