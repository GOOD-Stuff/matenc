#include <QString>
#include <QByteArray>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QVector>
#include <unistd.h>
#include <algorithm>
#include <qalgorithms.h>
#include <string>
#include <sys/stat.h>

#define SUCCESS 0
#define FAILURE 1

using namespace std;
static char *path_key;
static char *path_text;
static char *path_encr;

/*************** PROTOTYPES ****************/
static const QString get_keys(ifstream &file);
static const QString get_text(ifstream &file);
static const QString get_plain_text(const QString key,
                                   const QString clear_text);
static const QByteArray set_buff(const QString encr_str);
static const QVector<int> get_vect(QString key_str);
static const QVector<int> get_revers_mat(QVector<int> matrix);
static void clear_enters(QString &text);
static const QByteArray set_buff(const QString encr_str);
static int menu(int argc, char **argv);
/******************************************/

int main(int argc, char *argv[]){
    path_key = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_text = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_encr = (char*)calloc(MAX_INPUT, sizeof(char*));

    // Get path to files
    if( menu(argc, argv) != 0 )
        return -FAILURE;

    ifstream key_file(path_key);
    if( !key_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    ifstream encr_file(path_encr);
    if( !encr_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    const QString key_str  = get_keys(key_file);
    const QString encr_str = get_text(encr_file);
    QByteArray byte_str    = set_buff(encr_str);
    QVector<int> key_vect  = get_vect(key_str);
    QVector<int> rev_vect  = get_revers_mat(key_vect);


    return SUCCESS;
}

/**
 * @brief menu - Parse input arguments on keys and path to file;
 * @param argc - Count of input arguments;
 * @param argv - Input arguments;
 * @return SUCCESS - If all arguments is normal;
 *         FAILURE - If was error;
 */
static int menu(int argc, char **argv){
    string help = "\tYou must to use:\n"
                  "-k\t- file which contains key;\n"
                  "-t\t- file which contains plain text;\n"
                  "-e\t- file where will be contains encrypted text;\n"
                  "-h\t- this help view;\n";
    if( ( argc < 5 ) || ( argc > 7 ) ){
        cout << "\tYou doing something wrong!" << endl;
        cout << help << endl;
        return -FAILURE;
    }

    int opt;
    while( (opt = getopt(argc, argv, "k:t:e:h:")) != -1 ){
        switch(opt){
        case 'k':
            cout << "\tYour key file will be: ";
            strcpy(path_key, optarg);
            cout << path_key << endl;
            break;
        case 't':
            cout << "\tYour plaintext file will be: ";
            strcpy(path_text, optarg);
            cout << path_text << endl;
            break;
        case 'e':
            cout << "\tYour encrypted file will be: ";
            strcpy(path_encr, optarg);
            cout << path_encr << endl;
            break;
        case 'h':
            cout << help << endl;
            return -FAILURE;
        default:
            cout << "\tYou wroooong! Why?" << endl;
            cout << help << endl;
            return -FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * @brief get_key - return key words or phrase for encryption, length of alphabet
 * @param file    - file which contains key words
 * @return s_key_words - if all was successfully, string of key phrase;
 *         NULL        - if was error;
 */
static const QString get_keys(ifstream &file){
    struct stat info_file;
    stat(path_key, &info_file);  // get size of file

    if( info_file.st_size <= 0 ){
        printf("Why your %s is empty? (^o^)\r\n", path_key);
        return NULL;
    }

    char *key_words = (char*)calloc(info_file.st_size, sizeof(char*));
    if( key_words == NULL ){
        printf("Couldn't allocate memory for key words. Sorry (X_X)\r\n");
        return NULL;
    }

    file.read(key_words, info_file.st_size);
    if( !file ) {
        printf("Couldn't read file with keys. Sorry (*-*)\r\n");
        return NULL;
    }

    QString s_key_words(key_words);
   // clear_enters(s_key_words);

    free(key_words);

    return s_key_words;
}

/**
 * @brief get_text - return string of text for encryption
 * @param file     - file which contains not encrypted text
 * @return s_text - if all was successfully, string of not encrypted text for encryption;
 *         NULL        - if was error;
 */
static const QString get_text(ifstream &file){
    struct stat info_file;
    stat(path_encr, &info_file);

    if( info_file.st_size <= 0 ){   // Check size of file
        printf("Why your %s is empty? (^o^)\r\n", path_encr);
        return NULL;
    }

    char *text = (char*) calloc(info_file.st_size, sizeof(char*));
    if( text == NULL ){     // If we can't allocate memory
        printf("Couldn't allocate memory for plaintext. Sorry (X_X)\r\n");
        return NULL;
    }

    file.seekg(0, ios_base::beg);           // Set cursor to start of file
    file.read(text, info_file.st_size);
    if( !file ){
        printf("Couldn't read file with plaintext. Sorry (*-*)\r\n");
        return NULL;
    }

    QString s_text(text);
    clear_enters(s_text);

    free(text);
    return s_text;
}


/**
 * @brief set_buff - Fill array of bytes values from encr_str;
 * @param encr_str - string with text (bytes);
 * @return array of bytes;
 */
static const QByteArray set_buff(const QString encr_str){
    QByteArray result;

    quint8 tmp_byte_safe = 0;
    for( int i = 0; i < encr_str.toUtf8().size(); i++ ){
        tmp_byte_safe = encr_str.toUtf8().at(i);
        result.append(tmp_byte_safe);
    }

    return result;
}

/**
 * @brief get_vect - Translate from QString to QVector<int>
 * @param key_str
 * @return
 */
static const QVector<int> get_vect(QString key_str){
    QVector<int> numb_vect;
    QString tmp_str;
    for(auto iter_char = key_str.begin(); iter_char != key_str.end(); iter_char++){
        while( !iter_char->isSpace() && iter_char->isNumber() )
            tmp_str.append(*iter_char++);

        numb_vect.push_back(atoi(tmp_str.toStdString().c_str()));
        tmp_str.clear();
    }

    return numb_vect;
}

static const QVector<int> get_revers_mat(QVector<int> matrix){
    QVector<int> revert_matrix;
    int determinant = matrix[0];
    int row_length = sqrt(matrix.length());

    //for( int i = 0; i < row_length; i++ ){
    // Get dumb determinant of matrix;
        determinant = matrix[0] * (matrix[4] * matrix[8] - matrix[5] * matrix[7]) -
                      matrix[1] * (matrix[3] * matrix[8] - matrix[5] * matrix[6]) +
                      matrix[2] * (matrix[3] * matrix[7] - matrix[4] * matrix[6]);
        if( !determinant ){
            cout << "Error!!! You have a nondegenerate matrix!" << endl;
            exit(-FAILURE);
        }



    return revert_matrix;
}

/**
 * @brief clear_enters - Clear string only from \n symbols;
 * @param text         - String of text (key, alphabet, plain text);
 */
static void clear_enters(QString &text){
    int pos = 0;
    while( text.contains('\n') ) {
        pos = text.indexOf('\n');
        if( pos != -1 )
            text.remove(pos, 1);
    }
}
