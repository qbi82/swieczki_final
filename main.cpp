#include <iostream>
#include <iomanip>

#define MAX_DATE_LENGTH 20
#define MAX_FILENAME_LENGTH 50
#define MAX_LOG_LENGTH 100

struct StockData {
    char date[MAX_DATE_LENGTH];
    double open;
    double high;
    double low;
    double close;
    double volume;
};

int DEFAULT_CHART_HEIGHT = 50;
int DEFAULT_CHART_WEIGHT = 250;

void generateCandlestickCharts(StockData data[], const char* outputFile, int startIndex, int endIndex, int candleWidth, int chartHeight) {
    const char WHITE_CANDLE_BODY = 'O';
    const char BLACK_CANDLE_BODY = '#';
    const char CANDLE_SHADOW = '|';
    const char EMPTY_SPACE = ' ';

    FILE* file = fopen(outputFile, "w");
    if (file == nullptr) {
        std::cout << "Nie mozna otworzyc pliku do zapisu.\n";
        return;
    }

    int dataRange = endIndex - startIndex + 1;
    int startIndexInRange = (dataRange > candleWidth) ? dataRange - candleWidth : 0;

    double maxPrice = 0;
    double minPrice = 1.79769e+308; // Zamiennik DBL_MAX

    for (int i = startIndexInRange; i < dataRange; ++i) {
        maxPrice = (maxPrice > data[startIndex + i].high) ? maxPrice : data[startIndex + i].high;
        minPrice = (minPrice < data[startIndex + i].low) ? minPrice : data[startIndex + i].low;
    }

    double priceRange = maxPrice - minPrice;
    double priceStep = priceRange / chartHeight;

    for (double price = maxPrice; price >= minPrice; price -= priceStep) {
        fprintf(file, "%10.2f |", price);
        for (int i = startIndex; i <= endIndex; ++i) {
            if (price >= data[i].low && price <= data[i].high) {
                if (data[i].open < data[i].close) {
                    if (price >= data[i].open && price <= data[i].close) {
                        fprintf(file, "%c", WHITE_CANDLE_BODY);
                    } else if (price >= data[i].low && price <= data[i].open) {
                        fprintf(file, "%c", CANDLE_SHADOW);
                    } else if (price >= data[i].close && price <= data[i].high) {
                        fprintf(file, "%c", CANDLE_SHADOW);
                    } else {
                        fprintf(file, "%c", EMPTY_SPACE);
                    }
                } else {
                    if (price >= data[i].close && price <= data[i].open) {
                        fprintf(file, "%c", BLACK_CANDLE_BODY);
                    } else if (price >= data[i].low && price <= data[i].close) {
                        fprintf(file, "%c", CANDLE_SHADOW);
                    } else if (price >= data[i].open && price <= data[i].high) {
                        fprintf(file, "%c", CANDLE_SHADOW);
                    } else {
                        fprintf(file, "%c", EMPTY_SPACE);
                    }
                }
            } else {
                fprintf(file, "%c", EMPTY_SPACE);
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    std::cout << "Wygenerowano wykres swiecowy w pliku '" << outputFile << "'.\n";
}

void changeChartHeight(int& chartHeight) {
    std::cout << "Aktualna wysokosc wykresu: " << chartHeight << std::endl;
    std::cout << "Podaj nowa wysokosc wykresu: ";
    int newHeight;
    std::cin >> newHeight;
    chartHeight = newHeight;

}

void changeDateRange(int& startIndex, int& endIndex, int dataSize) {
    std::cout << "Aktualny indeks poczatkowy: " << startIndex << std::endl;
    std::cout << "Podaj nowy indeks poczatkowy (0-" << dataSize - 1 << "): ";
    std::cin >> startIndex;

    std::cout << "Aktualny indeks koncowy: " << endIndex << std::endl;
    std::cout << "Podaj nowy indeks koncowy (0-" << dataSize - 1 << "): ";
    std::cin >> endIndex;


    startIndex = (startIndex < 0) ? 0 : (startIndex >= dataSize) ? dataSize - 1 : startIndex;
    endIndex = (endIndex < 0) ? 0 : (endIndex >= dataSize) ? dataSize - 1 : endIndex;
}

void changeCandlestickWidth(int& candleWidth) {
    std::cout << "Wybierz szerokosc jednej swiecy:\n";
    std::cout << "1. Jeden tydzien\n";
    std::cout << "2. Jeden miesiac\n";
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            candleWidth = 7;
            break;
        case 2:
            candleWidth = 30;
            break;
        default:
            std::cout << "Niepoprawny wybor. Ustawiono domyslna wartosc (tydzien).\n";
            candleWidth = 7;
            break;
    }
}

int readCSV(const char* filename, StockData*& data) {
    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        std::cout << "Nie mozna otworzyc pliku!\n";
        return 0;
    }

    int dataSize = 0;
    const int defaultDataSize = 200;
    char line[MAX_LOG_LENGTH];
    fgets(line, MAX_LOG_LENGTH, file);

    while (fgets(line, MAX_LOG_LENGTH, file) != nullptr) {
        dataSize++;
    }

    fseek(file, 0, SEEK_SET);
    fgets(line, MAX_LOG_LENGTH, file);

    dataSize = (dataSize > defaultDataSize) ? defaultDataSize : dataSize;

    data = new StockData[dataSize];
    int index = 0;

    while (fgets(line, MAX_LOG_LENGTH, file) != nullptr && index < dataSize) {
        sscanf(line, "%19[^,],%lf,%lf,%lf,%lf,%lf", data[index].date, &data[index].open, &data[index].high, &data[index].low, &data[index].close, &data[index].volume);
        index++;
    }

    fclose(file);
    return dataSize;
}

void displayPartialChartAsCandlesticks(StockData data[], int startIdx, int endIdx, int chartHeight) {
    const char WHITE_CANDLE_BODY = 'O';
    const char BLACK_CANDLE_BODY = '#';
    const char CANDLE_SHADOW = '|';
    const char EMPTY_SPACE = ' ';

    double maxPrice = 0;

    double minPrice = 1.7976931348623157E+308;

    for (int i = startIdx; i <= endIdx; ++i) {
        maxPrice = (data[i].high > maxPrice) ? data[i].high : maxPrice;
        minPrice = (data[i].low < minPrice) ? data[i].low : minPrice;
    }

    double priceRange = maxPrice - minPrice;
    double priceStep = priceRange / chartHeight;

    if (endIdx - startIdx > chartHeight) {
        priceStep = priceRange / (endIdx - startIdx);
    }

    for (double price = maxPrice; price >= minPrice; price -= priceStep) {
        std::cout << std::setw(10) << price << " | ";

        for (int i = startIdx; i <= endIdx; ++i) {
            if (price >= data[i].low && price <= data[i].high) {
                if (data[i].open < data[i].close) {
                    if (price >= data[i].open && price <= data[i].close) {
                        std::cout << WHITE_CANDLE_BODY;
                    } else if (price >= data[i].low && price <= data[i].open) {
                        std::cout << CANDLE_SHADOW;
                    } else if (price >= data[i].close && price <= data[i].high) {
                        std::cout << CANDLE_SHADOW;
                    } else {
                        std::cout << EMPTY_SPACE;
                    }
                } else {
                    if (price >= data[i].close && price <= data[i].open) {
                        std::cout << BLACK_CANDLE_BODY;
                    } else if (price >= data[i].low && price <= data[i].close) {
                        std::cout << CANDLE_SHADOW;
                    } else if (price >= data[i].open && price <= data[i].high) {
                        std::cout << CANDLE_SHADOW;
                    } else {
                        std::cout << EMPTY_SPACE;
                    }
                }
            } else {
                std::cout << EMPTY_SPACE;
            }
        }
        std::cout << std::endl;
    }
}

void chooseOutputFile(char* filename) {
    std::cout << "Podaj nazwe pliku wyjsciowego (razem z rozszerzeniem .txt): ";
    std::cin >> filename;
}

void chooseInputFile(char* filename) {
    std::cout << "Podaj nazwe pliku wejsciowego (razem z rozszerzeniem .csv): ";
    std::cin >> filename;
}

void displayMenu() {
    std::cout << "==== MENU ====\n";
    std::cout << "g -  Wygeneruj wykres swiecowy\n";
    std::cout << "q - Wyjdz z programu\n";
    std::cout << "1. Wybierz plik wejsciowy\n";
    std::cout << "2. Wybierz plik wyjsciowy\n";
    std::cout << "3. Zmien wysokosc wykresu\n";
    std::cout << "4. Zmien szerokosc wykresu\n";
    std::cout << "5. Wyswietl kawalek wykresu(0-50)\n";
    std::cout << "Wybierz opcje: ";
}

void displayWelcomeMessage() {
    std::cout << "Witaj! Projekt autorstwa [Imie] [Nazwisko], indeks: [Twoj indeks]\n";
}

void saveUserActionToLog(const char* action) {
    FILE* logFile = fopen("user_actions.log", "a");
    if (logFile != nullptr) {
        time_t currentTime;
        time(&currentTime);
        fprintf(logFile, "%s: %s\n", ctime(&currentTime), action);
        fclose(logFile);
    } else {
        std::cout<<"Nie mozna otworzyc pliku logu."<<std::endl;
    }
}

int main() {
    char filename[MAX_FILENAME_LENGTH];
    displayWelcomeMessage();
    StockData* stockData = nullptr;
    int dataSize = readCSV("intc_us_data.csv", stockData);
    char outputfile[MAX_FILENAME_LENGTH] = "chart.txt";

    if (dataSize == 0) {
        return 1;
    }
    int startIndex = 0;
    int endIndex = dataSize - 1;
    bool startIndexUpdated = false;
    bool endIndexUpdated = false;
    char choice = '0';
    int zakres = 0;
    do {

        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 'g': {
                if (startIndexUpdated || endIndexUpdated) {
                    generateCandlestickCharts(stockData, outputfile, startIndex, endIndex, DEFAULT_CHART_WEIGHT, DEFAULT_CHART_HEIGHT);
                    saveUserActionToLog("Wybrano opcję: Generuj wykres");
                } else {
                    generateCandlestickCharts(stockData, outputfile, startIndex, dataSize - 1, DEFAULT_CHART_WEIGHT, DEFAULT_CHART_HEIGHT);
                    saveUserActionToLog("Wybrano opcję: Generuj wykres");
                }
                break;
            }
            case 'q':
                std::cout << "Zamykanie programu..." << std::endl;
                saveUserActionToLog("Wybrano opcję: Zakończ program");
                break;
            case '1':
                chooseInputFile(filename);
                dataSize = readCSV(filename, stockData);
                saveUserActionToLog("Wybrano opcję: Wybierz plik wejściowy");
                break;
            case '2':
                chooseOutputFile(outputfile);
                saveUserActionToLog("Wybrano opcję: Wybierz plik wyjściowy");
                break;
            case '3':
                changeChartHeight(DEFAULT_CHART_HEIGHT);
                generateCandlestickCharts(stockData, outputfile, startIndex, endIndex, DEFAULT_CHART_WEIGHT, DEFAULT_CHART_HEIGHT);
                saveUserActionToLog("Wybrano opcję: Zmień wysokość wykresu");
                break;
            case '4':
                std::cout << "Wybierz zakres: " << std::endl;
                std::cout << "1 - wedlug tygodni, miesiecy " << std::endl;
                std::cout << "2 - dowolny zakres: " << std::endl;
                std::cin >> zakres;
                switch (zakres) {
                    case 1:
                        changeCandlestickWidth(DEFAULT_CHART_WEIGHT);
                        generateCandlestickCharts(stockData, outputfile, startIndex, endIndex, DEFAULT_CHART_WEIGHT, DEFAULT_CHART_HEIGHT);
                        saveUserActionToLog("Wybrano opcję: Zmień szerokość wykresu");
                        break;
                    case 2:
                        changeDateRange(startIndex, endIndex, dataSize);
                        startIndexUpdated = true;
                        endIndexUpdated = true;
                        saveUserActionToLog("Wybrano opcję: Zmień początek i koniec zakresu dat");
                        break;
                    default:
                        std::cout << "niewlasciwy wybor!";
                        break;
                }
                break;
            case '5':
                displayPartialChartAsCandlesticks(stockData, 0, 50, DEFAULT_CHART_HEIGHT);
                saveUserActionToLog("Wybrano opcję: Wyświetl kawałek wykresu");
                break;

            default:
                std::cout << "Niepoprawny wybor. Sprobuj ponownie." << std::endl;
                saveUserActionToLog("Wybrano opcję: Niepoprawny wybór");
                break;
        }

    } while (choice != 'q');

    delete[] stockData;
    return 0;
}
