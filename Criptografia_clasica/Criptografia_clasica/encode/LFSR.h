#ifndef _LFSR_H_
#define _LFSR_H_

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#define BYTE unsigned char

class LFSR {
    private: std::string str_lfsr = "", polinom = "";
    private: std::vector<std::string> historial;
    private: bool isPrimitive;
    private: const BYTE XOR(const BYTE a, const BYTE b) {
        return (a == b ? '0' : '1');
    }
    private: const char XORAmongString(const std::string str) {
        size_t count = 0, pos = str.find_first_of('1');
        while (pos != std::string::npos) {
            count++;
            pos = str.find_first_of('1', pos + 1);
        }
        return (count % 2 ? '1' : '0');
    }
    private: const std::string InvertString(const std::string str) {
        std::string inv_str;
        for (char c : str)
            inv_str = c + inv_str;
        return inv_str;
    }
    private: const std::string ExtractChain(const std::string seed, const std::string poli) {
        std::string bits;
        size_t pos = poli.find_first_of('1');
        while (pos != std::string::npos) {
            bits = seed[poli.size() - 1 + pos] + bits;
            pos = poli.find_first_of('1', pos + 1);
        }
        return bits;
    }
    private: const std::string MakeNFSR(const std::string seed, const std::string poli) {
        this->historial.clear();
        const int size_seed = seed.size();
        const size_t n = (1 << size_seed) - 1;
        size_t i;
        std::string chain(seed), subchain;
        for (i = 0; i < n; i++) {
            subchain = chain.substr(0, size_seed);
            this->historial.push_back(std::string(subchain));
            chain = XORAmongString(ExtractChain(subchain, poli)) + chain;
           if (!chain.substr(0, seed.size()).compare(seed)) {
                this->isPrimitive = true;
                return InvertString(chain.substr(size_seed, chain.size() - size_seed));
            }
        }
        this->isPrimitive = !seed.compare(chain.substr(i, size_seed));
        return InvertString(chain.substr(size_seed, chain.size() - size_seed));
    }

    private: std::vector<std::string> GetCombinations(const size_t level){
        std::vector<std::string> comb;
        std::string str(level, '0'); comb.push_back(std::string(str));
        //std::cout << "Iniciando proceso\n";
        for (size_t i = 0; i < (1 << level) - 1; i++) {
            for (int j = 0; j < level; j++) 
                if (str[level - j - 1] == '0') {
                    str[level - j - 1] = '1';
                    break;
                } else 
                    str[level - j - 1] = '0';
            comb.push_back(std::string(str));
        }
        return comb;
    }
    private: size_t SearchTimes(const std::string comb){
        size_t times = 0;
        for (size_t i = 0; i < (this->str_lfsr.size() - (int)comb.size() + 1); i++)
            if (!comb.compare(this->str_lfsr.substr(i, comb.size())))
                times++;
        return times;
    }

    public: LFSR(const std::string seed, const std::string polinom) {
        if (seed.find('1') != std::string::npos) 
            this->str_lfsr = MakeNFSR(seed, polinom);
        else 
            this->str_lfsr = seed;
        this->polinom = polinom;
    }
    
    public: std::string getLFSR() {
        return this->str_lfsr;
    }
    public: void printReport(size_t level = 3) {
        if (this->isPrimitive) std::cout << "El polinomio otorgado es primitivo\n";
        else std::cout << "El polinomio otorgado no es primitivo\n";
        std::cout << "\nSecuencia de formacion: \n";
        for(size_t i = 0; i < this->historial.size() ; i++)
            std::cout << (i + 1) << ".-\t" << this->historial[i] << " : " << this->historial[i].back() << std::endl;
        std::cout << "LFSR: " << this->str_lfsr << std::endl;
        std::cout << "\nPostulados: \n";
        for (size_t i = 0; i < level; i++) {
            std::vector<std::string> comb = this->GetCombinations(i + 1);
            std::cout << "Nivel " << (i + 1) << std::endl;
            for (std::string c : comb)
                std::cout << "\t" << c << " = " << this->SearchTimes(c) << std::endl;
        }
    }
};

void example_LFSR() {
    std::string seed = "10101", pol = "101";//100001
    std::cout << "Semilla: " << seed << "\nPolinomio [" << pol << "]\n";
    LFSR lfsr(seed, pol);
    lfsr.printReport(3);
}

#endif