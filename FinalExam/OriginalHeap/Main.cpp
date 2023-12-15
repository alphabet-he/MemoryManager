#include "HeapManagerUnitTest.h"

int main() {
    bool heapPassed = HeapManager_UnitTest();
    if (heapPassed)
        printf("heap passed\n");
    else
        printf("heap FAILED\n");
}