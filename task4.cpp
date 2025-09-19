#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <queue>

class Huffman
{
    // специальная структура для хранения пары "группа символов (>=1)" и "(суммарная) частота"
    struct CharSetFrequency
    {
        std::string chars;
        int freq;

        bool operator<(const CharSetFrequency &other) const
        {
            // для лексиграфического сравнения комплексных объектов используем tie
            return std::tie(freq, chars) > std::tie(other.freq, other.chars);
        }
    };

public:
    static std::unordered_map<char, std::string> encode(const std::string &text);
    static std::string decode(const std::string &text, const std::unordered_map<char, std::string> &huffman_encoding);
};

std::unordered_map<char, std::string> Huffman::encode(const std::string &text)
{
    // проходимся и считаем частоты
    std::unordered_map<char, int> char_freqs;
    for (auto c : text)
    {
        char_freqs[c]++;
    }

    // заводим первичные CharSetFrequency
    std::vector<CharSetFrequency> freqs;
    for (auto char_freq : char_freqs)
    {
        freqs.push_back({std::string(1, char_freq.first), char_freq.second});
    }

    std::unordered_map<char, std::string> huffman_code;

    // обработка случая с одним символом
    if (freqs.size() == 1)
    {
        huffman_code[freqs[0].chars[0]] = "0";
        return huffman_code;
    }

    // заводим очередь первичных CharSetFrequency
    std::priority_queue<CharSetFrequency> q(freqs.begin(), freqs.end());

    /*
    с помощью алгоритма Хаффмана как бы строим дерево, "сшивая" CharSetFrequency и CharSetFrequency,
    меняя при этом кодировку в huffman_code, и в конце добавляем в очередь новый CharSetFrequency,
    в котором строки конкатенированны(чтобы проходится потом по символам), а частоты сложены
    */
    while (q.size() >= 2)
    {
        auto first = q.top();
        q.pop();
        auto second = q.top();
        q.pop();

        for (auto c : first.chars)
        {
            huffman_code[c] = "0" + huffman_code[c];
        }

        for (auto c : second.chars)
        {
            huffman_code[c] = "1" + huffman_code[c];
        }

        q.push({first.chars + second.chars, first.freq + second.freq});
    }
    // возвращаем словарь (символ, код)
    return huffman_code;
}
// декодирование
std::string Huffman::decode(const std::string &text, const std::unordered_map<char, std::string> &huffman_encoding)
{
    size_t len = text.size();
    size_t pos = 0;
    std::string decoded_text;
    while (pos < len)
    {
        // ищем вхождение какого либо кодового слова из полученного кода
        for (auto &encoded : huffman_encoding)
        {
            // сравниваем подстроку и кодовое слово
            if (text.substr(pos, encoded.second.size()) == encoded.second)
            {
                decoded_text += encoded.first;
                pos += encoded.second.size();
                break;
            }
        }
    }
    return decoded_text;
}

int main()
{
    std::cout << "-----------------Huffman Code-----------------" << std::endl
              << std::endl;

    std::cout << "---------------Enter your text:----------------" << std::endl;

    // вводим строку с клавиатуры
    std::string text;
    std::cin >> text;
    std::cout << "-----------------------------------------------" << std::endl;

    // кодируем
    auto huffman_encoding = Huffman::encode(text);

    // формируем кодовое слово
    std::string encoded_text;
    for (auto c : text)
    {
        encoded_text += huffman_encoding[c];
    }

    // вывод

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Symbols: " << huffman_encoding.size() << " Result size: " << encoded_text.size() << " symbol(s)" << std::endl
              << std::endl;

    std::cout << "Code: " << std::endl
              << std::endl;
    for (auto &encoded : huffman_encoding)
    {
        std::cout << encoded.first << ": " << encoded.second << std::endl;
    }
    std::cout << "Coded text: " << std::endl;

    std::cout << encoded_text << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}