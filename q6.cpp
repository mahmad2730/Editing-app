#include <iostream>
#include <fstream>
using namespace std;

int clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

void renderMatrix(int matrix[64][64][3]) {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            cout << "\033[48;2;" << matrix[i][j][0] << ";" << matrix[i][j][1] << ";" << matrix[i][j][2] << "m  \033[0m";
        }
        cout << endl;
    }
}

void loadImages(int images[5][64][64][3]) {
    ifstream file("images.txt");
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return;
    }
    
    for (int img = 0; img < 5; img++) {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                file >> images[img][i][j][0] >> images[img][i][j][1] >> images[img][i][j][2];
            }
        }
    }
    file.close();
}

void convertMonochrome(int matrix[64][64][3]) {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int avg = (matrix[i][j][0] + matrix[i][j][1] + matrix[i][j][2]) / 3;
            matrix[i][j][0] = avg;
            matrix[i][j][1] = avg;
            matrix[i][j][2] = avg;
        }
    }
}

void changeBrightness(int matrix[64][64][3], int k) {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix[i][j][0] = clamp(matrix[i][j][0] + k);
            matrix[i][j][1] = clamp(matrix[i][j][1] + k);
            matrix[i][j][2] = clamp(matrix[i][j][2] + k);
        }
    }
}

void changeContrast(int matrix[64][64][3], bool increase) {
    float factor = increase ? 1.2 : 0.8;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix[i][j][0] = clamp((int)(factor * (matrix[i][j][0] - 128) + 128));
            matrix[i][j][1] = clamp((int)(factor * (matrix[i][j][1] - 128) + 128));
            matrix[i][j][2] = clamp((int)(factor * (matrix[i][j][2] - 128) + 128));
        }
    }
}

void rotateClockwise(int matrix[64][64][3]) {
    int temp[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            temp[j][63 - i][0] = matrix[i][j][0];
            temp[j][63 - i][1] = matrix[i][j][1];
            temp[j][63 - i][2] = matrix[i][j][2];
        }
    }
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix[i][j][0] = temp[i][j][0];
            matrix[i][j][1] = temp[i][j][1];
            matrix[i][j][2] = temp[i][j][2];
        }
    }
}

void applyBlur(int matrix[64][64][3]) {
    int temp[64][64][3];
    int kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int totalWeight = 16;
    
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            if (i == 0 || i == 63 || j == 0 || j == 63) {
                temp[i][j][0] = matrix[i][j][0];
                temp[i][j][1] = matrix[i][j][1];
                temp[i][j][2] = matrix[i][j][2];
                continue;
            }
            
            int sum0 = 0, sum1 = 0, sum2 = 0;
            for (int ki = -1; ki <= 1; ki++) {
                for (int kj = -1; kj <= 1; kj++) {
                    int weight = kernel[ki + 1][kj + 1];
                    sum0 += matrix[i + ki][j + kj][0] * weight;
                    sum1 += matrix[i + ki][j + kj][1] * weight;
                    sum2 += matrix[i + ki][j + kj][2] * weight;
                }
            }
            temp[i][j][0] = sum0 / totalWeight;
            temp[i][j][1] = sum1 / totalWeight;
            temp[i][j][2] = sum2 / totalWeight;
        }
    }
    
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix[i][j][0] = temp[i][j][0];
            matrix[i][j][1] = temp[i][j][1];
            matrix[i][j][2] = temp[i][j][2];
        }
    }
}

void applySharpen(int matrix[64][64][3]) {
    int temp[64][64][3];
    int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            if (i == 0 || i == 63 || j == 0 || j == 63) {
                temp[i][j][0] = matrix[i][j][0];
                temp[i][j][1] = matrix[i][j][1];
                temp[i][j][2] = matrix[i][j][2];
                continue;
            }
            
            int sum0 = 0, sum1 = 0, sum2 = 0;
            for (int ki = -1; ki <= 1; ki++) {
                for (int kj = -1; kj <= 1; kj++) {
                    int weight = kernel[ki + 1][kj + 1];
                    sum0 += matrix[i + ki][j + kj][0] * weight;
                    sum1 += matrix[i + ki][j + kj][1] * weight;
                    sum2 += matrix[i + ki][j + kj][2] * weight;
                }
            }
            temp[i][j][0] = clamp(sum0);
            temp[i][j][1] = clamp(sum1);
            temp[i][j][2] = clamp(sum2);
        }
    }
    
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix[i][j][0] = temp[i][j][0];
            matrix[i][j][1] = temp[i][j][1];
            matrix[i][j][2] = temp[i][j][2];
        }
    }
}

int main() {
    int images[5][64][64][3];
    int workingMatrix[64][64][3];
    
    loadImages(images);
    
    while (true) {
        cout << "\n=== Main Menu ===" << endl;
        cout << "Select an image (1-5) or 0 to exit: ";
        int imgChoice;
        cin >> imgChoice;
        
        if (imgChoice == 0) break;
        if (imgChoice < 1 || imgChoice > 5) {
            cout << "Invalid choice!" << endl;
            continue;
        }
        
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                workingMatrix[i][j][0] = images[imgChoice - 1][i][j][0];
                workingMatrix[i][j][1] = images[imgChoice - 1][i][j][1];
                workingMatrix[i][j][2] = images[imgChoice - 1][i][j][2];
            }
        }
        
        while (true) {
            cout << "\n=== Analysis Menu ===" << endl;
            cout << "1. Display Image" << endl;
            cout << "2. Convert to Monochrome" << endl;
            cout << "3. Change Brightness" << endl;
            cout << "4. Change Contrast" << endl;
            cout << "5. Rotate 90 Clockwise" << endl;
            cout << "6. Apply Blur" << endl;
            cout << "7. Apply Sharpen" << endl;
            cout << "8. Reset Image" << endl;
            cout << "0. Back to Main Menu" << endl;
            cout << "Choice: ";
            
            int choice;
            cin >> choice;
            
            if (choice == 0) break;
            
            if (choice == 1) {
                renderMatrix(workingMatrix);
            } else if (choice == 2) {
                convertMonochrome(workingMatrix);
                renderMatrix(workingMatrix);
            } else if (choice == 3) {
                int k;
                cout << "Enter brightness change (-255 to 255): ";
                cin >> k;
                changeBrightness(workingMatrix, k);
                renderMatrix(workingMatrix);
            } else if (choice == 4) {
                cout << "Increase (1) or Decrease (0) contrast: ";
                int inc;
                cin >> inc;
                changeContrast(workingMatrix, inc == 1);
                renderMatrix(workingMatrix);
            } else if (choice == 5) {
                rotateClockwise(workingMatrix);
                renderMatrix(workingMatrix);
            } else if (choice == 6) {
                applyBlur(workingMatrix);
                renderMatrix(workingMatrix);
            } else if (choice == 7) {
                applySharpen(workingMatrix);
                renderMatrix(workingMatrix);
            } else if (choice == 8) {
                for (int i = 0; i < 64; i++) {
                    for (int j = 0; j < 64; j++) {
                        workingMatrix[i][j][0] = images[imgChoice - 1][i][j][0];
                        workingMatrix[i][j][1] = images[imgChoice - 1][i][j][1];
                        workingMatrix[i][j][2] = images[imgChoice - 1][i][j][2];
                    }
                }
                cout << "Image reset!" << endl;
            } else {
                cout << "Invalid choice!" << endl;
            }
        }
    }
    
    return 0;
}