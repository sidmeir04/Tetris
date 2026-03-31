#ifndef SRS_TABLE
#define SRS_TABLE

const std::vector<std::vector<std::vector<int>>> JLSTZ_KICKS = {
    { {0, 0}, {-1, 0}, {-1, -1}, {0,  2}, {-1,  2} }, // 0 -> 1
    { {0, 0}, { 1, 0}, { 1,  1}, {0, -2}, { 1, -2} }, // 1 -> 0
    { {0, 0}, { 1, 0}, { 1,  1}, {0, -2}, { 1, -2} }, // 1 -> 2
    { {0, 0}, {-1, 0}, {-1, -1}, {0,  2}, {-1,  2} }, // 2 -> 1
    { {0, 0}, { 1, 0}, { 1, -1}, {0,  2}, { 1,  2} }, // 2 -> 3
    { {0, 0}, {-1, 0}, {-1,  1}, {0, -2}, {-1, -2} }, // 3 -> 2
    { {0, 0}, {-1, 0}, {-1,  1}, {0, -2}, {-1, -2} }, // 3 -> 0
    { {0, 0}, { 1, 0}, { 1, -1}, {0,  2}, { 1,  2} }  // 0 -> 3
};
    
const std::vector<std::vector<std::vector<int>>> I_KICKS = {
    { {0, 0}, {-2, 0}, { 1, 0}, {-2,  1}, { 1, -2} }, // 0 -> 1
    { {0, 0}, { 2, 0}, {-1, 0}, { 2, -1}, {-1,  2} }, // 1 -> 0
    { {0, 0}, {-1, 0}, { 2, 0}, {-1, -2}, { 2,  1} }, // 1 -> 2
    { {0, 0}, { 1, 0}, {-2, 0}, { 1,  2}, {-2, -1} }, // 2 -> 1
    { {0, 0}, { 2, 0}, {-1, 0}, { 2, -1}, {-1,  2} }, // 2 -> 3
    { {0, 0}, {-2, 0}, { 1, 0}, {-2,  1}, { 1, -2} }, // 3 -> 2
    { {0, 0}, { 1, 0}, {-2, 0}, { 1,  2}, {-2, -1} }, // 3 -> 0
    { {0, 0}, {-1, 0}, { 2, 0}, {-1, -2}, { 2,  1} }  // 0 -> 3
};

int getRotationIndex(int prevRotation, int nextRotation) {
    if (prevRotation == 0 && nextRotation == 1) return 0;
    if (prevRotation == 1 && nextRotation == 0) return 1;
    if (prevRotation == 1 && nextRotation == 2) return 2;
    if (prevRotation == 2 && nextRotation == 1) return 3;
    if (prevRotation == 2 && nextRotation == 3) return 4;
    if (prevRotation == 3 && nextRotation == 2) return 5;
    if (prevRotation == 3 && nextRotation == 0) return 6;
    if (prevRotation == 0 && nextRotation == 3) return 7;
    return 0;
}

std::vector<std::vector<int>> getOffests(PieceState currentPiece, int lastRotation){
    if(currentPiece.piece==0){
        return I_KICKS[ getRotationIndex(lastRotation, currentPiece.rotation)];
    }
    return JLSTZ_KICKS[getRotationIndex(lastRotation, currentPiece.rotation)];
}

#endif