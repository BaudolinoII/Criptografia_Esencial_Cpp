
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <utility>
#include <functional>
//#include <Windows.h>

class EurekaMch {
    private: std::unordered_map<char, size_t> map;
    private: std::string constColl, collection;
    private: int* key_hill = nullptr, * inv_key_hill = nullptr;
    private: size_t lv_hill = 0, sqr_lv = 0;

    private: int mod(int n, size_t m) {
        if (n >= 0)
            return n % m;
        while (n < 0)
            n += m;
        return n;
    }
    private: void collectString(std::string& str) {
        for (size_t i = 0; i < str.size(); i++) {
            //std::cout  << "Analizando la letra: " << str[i];
            size_t found = str.find_first_of(str[i]);
            found = str.find_first_of(str[i], found + 1);//Encuentra la letra repetida
            while (found != std::string::npos) {
                //std::cout << "Letra " << str[i] <<" repetida en: " << found;
                str.erase(found, 1);
                found = str.find_first_of(str[i], found);//Encuentra si hay mas letras repetidas
            }
            //std::cout << std::endl;
        }
    }
    private: void toUpperCase(std::string& text) {
        for (size_t i = 0; i < text.size(); i++)
            if (text[i] > 0x40 && text[i] < 0x5B)
                text[i] &= 0xDF;
            else if (text[i] == 0xA4 || text[i] == 0xA5)// ñ o Ñ
                text[i] = 'n'; //representa una Ñ
    }

    private: int  determinate(int* mat, size_t lv, bool is_det = true) {
        if (lv == 2) {
            //std::wcout << mat[0] << L" * " << mat[3] << L" - " << mat[1] << L" * " << mat[2] << " = " << (mat[0]*mat[3] - mat[1]*mat[2]) << std::endl;
            return mat[0] * mat[3] - mat[1] * mat[2];
        }
        if (lv == 1)
            return mat[0];
        int det = 0;
        int temp = 0;
        for (size_t i = 0; i < lv; i++) {
            //temp = ( i%2 ? -1 : 1) * determinate(mkSubArray(mat,i),lv - 1);
            //std::cout << "Determinante parcial: "<< det << " += " << (int)temp << " * " << mat[i] << std::endl;
            //det += temp * mat[i];
            det += (i % 2 ? -1 : 1) * determinate(mkSubArray(mat, i), lv - 1) * (is_det ? mat[i] : 1);
        }
        //std::cout << "Determinante Total: " << det << std::endl;
        return det;
    }
    private: int* msgToIntArray(std::string msg) {
        const size_t sizeofArray = msg.size() + (msg.size() % this->lv_hill);
        int* mat = new int[sizeofArray];
        this->toUpperCase(msg);
        for (size_t i = 0; i < sizeofArray; i++)
            if (i <= msg.size())
                mat[i] = this->getValue(msg[i]);
            else
                mat[i] = 23;
        return mat;
    }
    private: int* mkSubArray(int* array, const size_t raw, const size_t col = 0) {
        int* subArray = new int[(this->lv_hill - 1) * (this->lv_hill - 1)];
        int index = 0;
        //std::wcout << "SubArreglo\n";
        for (size_t i = 0; i < this->lv_hill; i++)
            if (i == col) {}
            else {
                for (size_t j = 0; j < this->lv_hill; j++)
                    if (j != raw) {
                        //std::wcout << array[this->lv_hill*i + j] << L" ";
                        subArray[index] = array[this->lv_hill * i + j];
                        index++;
                    }
                //std::wcout << std::endl;
            }
        return subArray;
    }

    private: std::string msgToString(int* msg, size_t sizeofArray) {
        std::string str;
        //std::cout << "Tamanio solicitado: " << sizeofArray << std::endl;
        for (size_t i = 0; i < sizeofArray; i++)
            str.append(1, this->getChar(msg[i]));
        return str;
    }
    private: void set_InvKey(int scl) {
        this->inv_key_hill = new int[this->sqr_lv];
        for (size_t i = 0; i < this->lv_hill; i++)
            for (size_t j = 0; j < this->lv_hill; j++) {
                //int value = ((i*this->lv_hill + j)% 2 ? -1 : 1) * determinate(mkSubArray(this->key_hill, j, i), this->lv_hill - 1, false);
                //std::cout << value << " * " << scl << " = " << value * scl << " mod " << map.size() << " = "; 
                this->inv_key_hill[j * this->lv_hill + i] = mod(((i * this->lv_hill + j) % 2 ? -1 : 1) * determinate(mkSubArray(this->key_hill, j, i), this->lv_hill - 1, false) * scl, map.size());
                //std::cout << this->inv_key_hill[j*this->lv_hill + i] << std::endl;
            }
        //this->inv_key_hill[j*this->lv_hill + i]
    }
    private: int* mulMsg_Matrix(int* key, int* msg, size_t col) {
        int* mul = new int[this->lv_hill * col];
        for (size_t i = 0; i < this->lv_hill * col; i++)
            mul[i] = 0;
        for (size_t i = 0; i < this->lv_hill; i++)
            for (size_t j = 0; j < this->lv_hill * col; j++) {
                //std::cout << j << ".- " << mul[j] << " += " << key[(j % this->lv_hill)*this->lv_hill + i] << " * " << msg[i] << std::endl;
                mul[j] += key[(j % this->lv_hill) * this->lv_hill + i] * msg[i];
            }

        for (size_t i = 0; i < this->lv_hill * col; i++) {
            mul[i] = mod(mul[i], this->map.size());
            //std::cout << mul[i] << " mod by " << map.size() << std::endl;
        }
        return mul;
    }

    public: EurekaMch(std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
        toUpperCase(alpha);
        this->collection = alpha;
        this->constColl = alpha;
        for (char c : this->collection) this->map[c] = this->map.size();
    }

    public: void setKeyHill(std::string str, size_t lv) {
        this->toUpperCase(str);
        this->lv_hill = lv;
        this->sqr_lv = lv * lv;
        if (str.size() < this->sqr_lv) {
            std::cout << "La cadena no puede abarcar el nivel correspondiente: " << str.size() << "/" << (this->sqr_lv) << std::endl;
            this->key_hill = nullptr;
            this->lv_hill = 0;
            this->sqr_lv = 0;
            return;
        }
        this->key_hill = new int[this->sqr_lv];
        for (size_t i = 0; i < this->lv_hill; i++)
            for (size_t j = 0; j < this->lv_hill; j++)
                this->key_hill[this->lv_hill * i + j] = this->getValue(str[this->lv_hill * i + j]);
    }
    public: void setKey(std::string key) {
        toUpperCase(key);
        key.append(this->constColl);
        this->collectString(key);
        this->collection = key;
        for (char c : this->collection) this->map[c] = this->map.size();
    }

    public: void resetAlphabeth() {
        this->collection = this->constColl;
        for (char c : this->collection) this->map[c] = this->map.size();
    }
    public: void printAlphabeth() {
        for (wchar_t c : this->collection) std::wcout << c;
        std::cout << std::endl;
    }
    public: void printKeyMatrix() {
        std::wcout << "Matriz de encriptacion\n";
        for (size_t i = 0; i < this->lv_hill; i++) {
            for (size_t j = 0; j < this->lv_hill; j++)
                std::wcout << (this->key_hill[i * this->lv_hill + j]) << " ";
            std::wcout << std::endl;
        }
        int true_det = this->determinate(this->key_hill, this->lv_hill);
        int det = true_det % this->map.size();
        size_t invD = getDmod1(det);
        std::wcout << "Determinante: " << true_det << " % " << this->map.size() << " = " << det << std::endl;
        std::wcout << "Inverso Entero: " << invD << std::endl;
        this->set_InvKey(invD);
        if (invD) {
            std::wcout << "Matriz de Desencriptacion\n";
            for (size_t i = 0; i < this->lv_hill; i++) {
                for (size_t j = 0; j < this->lv_hill; j++)
                    std::wcout << (this->inv_key_hill[i * this->lv_hill + j]) << " ";
                std::wcout << std::endl;
            }
        }
    }
    public: void sliceAlphabeth(size_t i) {
        i %= this->collection.size();
        char aux;
        for (size_t j = 0; j < i; j++) {
            aux = this->collection[0];
            this->collection = this->collection.substr(1, 37);
            this->collection.append(1, aux);
        }
        for (char c : this->collection) this->map[c] = this->map.size();
    }
    public: wchar_t getChar(const size_t i) {
        //std::cout << "Token Solicitado: " << i << std::endl;
        return this->collection[i];
    }
    public: size_t getValue(const wchar_t c) {
        return this->collection.find_first_of(c);
    }
    public: size_t getSize() {
        return this->map.size();
    }
    public: size_t getDmod1(size_t e) {
        for (size_t i = 2; i < this->map.size(); i++)
            if (i * e % this->map.size() == 1)
                return i;
        return 0;
    }

    public: std::string encriptString(std::string msg) {
        std::string dmsg;
        toUpperCase(msg);
        //std::cout << msg << std::endl;
        for (char c : msg) {
            //std::cout << c << " ";
            dmsg.append(1, this->getChar((c == 32 ? 24 : c - 65)));
        }
        std::cout << std::endl;
        return dmsg;
    }
    public: std::string dencriptString(std::string dmsg) {
        std::string msg;
        toUpperCase(msg);
        for (char c : dmsg) {
            msg.append(1, this->getValue(c) + 65);
        }
        return msg;
    }

    public: std::string encriptSingleSC(std::string msg, const std::string key) {
        size_t currVal, keyVal;
        toUpperCase(msg);
        std::string dmsg;
        for (size_t i = 0; i < msg.size(); i++) {
            currVal = (msg[i] == 32 ? 23 : msg[i] - 65);
            keyVal = (key[i % key.size()] == 32 ? 23 : key[i % key.size()] - 65);
            dmsg.append(1, (currVal + keyVal) % this->map.size() + 65);
        }
        return dmsg;
    }
    public: std::string dencriptSingleSC(std::string msg, const std::string key) {
        int currVal, keyVal;
        std::string dmsg;
        for (size_t i = 0; i < msg.size(); i++) {
            currVal = (msg[i] == 32 ? 23 : msg[i] - 65);
            keyVal = (key[i % key.size()] == 32 ? 23 : key[i % key.size()] - 65);
            dmsg.append(1, mod(currVal - keyVal, this->map.size()) + 65);
        }
        return dmsg;
    }

    public: std::string encriptHill(std::string msg, size_t lv) {
        size_t det = this->determinate(this->key_hill, this->lv_hill) % this->map.size();
        size_t invD = getDmod1(det);
        if (invD) {
            //this->set_InvKey(invD);
            int* matMsg = this->msgToIntArray(msg);
            int* dmsg = this->mulMsg_Matrix(this->key_hill, matMsg, (msg.size() + (msg.size() % lv)) / lv);
            //std::wcout << this->msgToString(matMsg, msg.size() + (msg.size() % lv)) << " | ";
            return this->msgToString(dmsg, msg.size() + (msg.size() % lv));
        }
        else
            std::wcout << L"No se podra decifrar el mensaje, porque no existe inverso determinante para " << det << L" en mod" << this->map.size() << std::endl;
        return "";
    }
    public: std::string dencriptHill(std::string msg, size_t lv) {
        size_t det = this->determinate(this->key_hill, this->lv_hill) % this->map.size();
        size_t invD = getDmod1(det);
        if (invD) {
            this->set_InvKey(invD);
            int* matMsg = this->msgToIntArray(msg);
            int* dmsg = this->mulMsg_Matrix(this->inv_key_hill, matMsg, (msg.size() + (msg.size() % lv)) / lv);
            //std::wcout << this->msgToString(matMsg, msg.size() + (msg.size() % lv)) << " | ";
            return this->msgToString(dmsg, msg.size() + (msg.size() % lv));
        }
        else
            std::wcout << L"No se puede decifrar el mensaje, porque no existe inverso determinante para " << det << L" en mod" << this->map.size() << std::endl;
        return "";
    }
};



void example_hamil() {
    EurekaMch classic_ceasar, key_ceasar;

    std::string spanish_alphanumeric("ABCDEFGHIJKLMNnOPQRSTUVWXYZ0123456789");
    EurekaMch sp_aplnum(spanish_alphanumeric);

    //std::cout << "-4 mod 37 = " << sp_aplnum.mod(-4,37) << std::endl;
    //std::cout << "-4 % 37 = " << (-4 % 37) << std::endl;

    classic_ceasar.sliceAlphabeth(3);

    std::string msgcc = "EMPERADOR";
    std::cout << "Encriptacion Cesar Clasico\n";
    std::cout << "Mensaje original: " << msgcc << std::endl;
    std::string dmsgcc = classic_ceasar.encriptString(msgcc);
    std::cout << "Mensaje encriptado: " << dmsgcc << std::endl;
    std::string back_msgcc = classic_ceasar.dencriptString(dmsgcc);
    std::cout << "Mensaje recuperado: " << back_msgcc << std::endl;

    std::string msgkc = "STARWARSLAVENGANZADELOSSITH";
    std::string key_c = "SECURITY";
    key_ceasar.sliceAlphabeth(3);
    key_ceasar.setKey(key_c);

    std::cout << "\n\nEncriptacion Cesar con Clave\n";
    std::cout << "Clave: " << key_c << "\nMensaje original: " << msgkc << std::endl;
    std::string dmsgkc = key_ceasar.encriptString(msgkc);
    std::cout << "Mensaje encriptado: " << dmsgkc << std::endl;
    std::string back_msgkc = key_ceasar.dencriptString(dmsgkc);
    std::cout << "Mensaje recuperado: " << back_msgkc << std::endl;

    std::string msguc = "SUPERMARIOBROS";
    std::string key_u = "SEGURIDAT";

    key_ceasar.resetAlphabeth();

    std::cout << "\n\nEncriptacion Cesar con Clave Unica\n";
    std::cout << "Clave: " << key_u << "\nMensaje original: " << msguc << std::endl;
    std::string dmsguc = key_ceasar.encriptSingleSC(msguc, key_u);
    std::cout << "Mensaje encriptado: " << dmsguc << std::endl;
    std::string back_msguc = key_ceasar.dencriptSingleSC(dmsguc, key_u);
    std::cout << "Mensaje recuperado: " << back_msguc << std::endl;

    std::cout << "\n\nEncriptacion Hill\nAlfabeto usado: ";
    sp_aplnum.printAlphabeth();
    sp_aplnum.setKeyHill("KILOWATTS", 3);
    sp_aplnum.printKeyMatrix();
    std::string msg("MKR");

    std::cout << "Mensaje original: " << msg << std::endl;
    std::string dmsg = sp_aplnum.encriptHill(msg, 3);
    std::cout << "Mensaje encriptado: " << dmsg << std::endl;
    std::string back_msg = sp_aplnum.dencriptHill(dmsg, 3);
    std::cout << "Mensaje recuperado: " << back_msg << std::endl;
}