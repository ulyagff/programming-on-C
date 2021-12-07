#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BASE 1000000000

typedef struct {
    uint32_t *number;
    size_t count; //всего непустых разрядов
    size_t size;  //всего разрядов
} Uint1024_t;

int compare(Uint1024_t *x, Uint1024_t *y) {
    if (x->count < y->count) return -1;
    else if (x->count == y->count) {
        for (int i = x->count - 1; i > -1; i--) {
            if (x->number[i] < y->number[i]) return -1;
            else if (x->number[i] > y->number[i]) return 1;
        }
        return 0;
    } else return 1;
}

Uint1024_t *init() {
    Uint1024_t *uint1024_t = (Uint1024_t *) malloc(sizeof(Uint1024_t));
    uint1024_t->size = 35;
    uint1024_t->count = 0;
    uint1024_t->number = (uint32_t *) malloc(uint1024_t->size * sizeof(uint32_t));
    memset(uint1024_t->number, 0, 35);
    return uint1024_t;
}

void _swap(uint8_t *array, size_t size) {
    uint8_t temp;
    for (size_t i = 0, j = size - 1; i < size / 2; ++i, --j) {
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

Uint1024_t *new_uint1024_t(const uint8_t *str) {
    const size_t BUF_SIZE = 9;

    Uint1024_t *uint1024_t = init();

    uint8_t chunk[BUF_SIZE + 1];
    memset(chunk, 0, (BUF_SIZE + 1) * sizeof(uint8_t));

    size_t next = 0;
    size_t index = 0;
    size_t length = strlen(str);

    for (size_t i = length - 1; i != -1; --i) {
        chunk[index++] = str[i];
        if (index == BUF_SIZE) {
            index = 0;
            _swap(chunk, BUF_SIZE);
            uint1024_t->number[next++] = atoi(chunk);
            uint1024_t->count++;
        }
    }
    if (index != 0) {
        chunk[index] = '\0';
        _swap(chunk, index);
        uint1024_t->number[next++] = atoi(chunk);
        uint1024_t->count++;
    }
    return uint1024_t;
}


void printf_value(Uint1024_t *x) {
    if (x->count == 0) {
        printf("null\n");
        return;
    }

    for (; x->count > 0; --x->count) {
        if (x->number[x->count - 1] != 0) {
            break;
        }
    }

    if (x->count == 0) {
        printf("%i", 0);
        return;
    }

    int i = x->count - 1;
    if (x->number[i]) printf("%u", x->number[i--]);
    for (; i != -1; --i) printf("%.9u", x->number[i]);
    putchar('\n');
}

Uint1024_t *from_uint(unsigned int x) {
    Uint1024_t *uint1024_t = init();
    uint1024_t->number[1] = x / BASE;
    uint1024_t->number[0] = x % BASE;
    uint1024_t->count = x / BASE > 0 ? 2 : 1;

    return uint1024_t;
}

void scanf_value(Uint1024_t *x) {
    char *str;
    str = (char *) (malloc(315 * sizeof(char)));
    scanf("%s", str);
    Uint1024_t *a = new_uint1024_t(str);
    x->count = a->count;
    x->size = 35;
    x->number = a->number;
}

void count_many(Uint1024_t *x, Uint1024_t *y) {
    if (x->count < y->count) {
        x->count = y->count;
    }
}

Uint1024_t add_op(Uint1024_t x, Uint1024_t y) {
    count_many(&x, &y);
    Uint1024_t * ans = init();
    ans->count = x.count;
    unsigned int carry = 0;
    for (size_t i = 0; i < x.count; i++) {
        uint32_t temp = carry + x.number[i] + y.number[i];
        ans->number[i] = temp % BASE;
        carry = temp / BASE;
    }
    if (carry && ans->count == 35) {
        printf("Undefined Behavior");
        exit(1);
    }

    if (carry) ans->number[ans->count++] = carry;

    return *ans;
}

Uint1024_t subtr_op(Uint1024_t x, Uint1024_t y) {
    if (compare(&x, &y) == -1) {
        printf("Undefined Behavior");
        exit(1);
    }
    else {
        Uint1024_t * ans = init();
        uint8_t carry = 0;
        ans->count = x.count;
        for (size_t i = 0; i < x.count; ++i) {
            uint32_t temp = (BASE + x.number[i]) - (carry + y.number[i]);
            ans->number[i] = temp % BASE;
            carry = (temp >= BASE) ? 0 : 1;
        }
        return *ans;
    }
}

Uint1024_t mult_op(Uint1024_t x, Uint1024_t y) {
    Uint1024_t *uint1024_t = init();

    for (int i = 0; i < x.count; i++)
        for (int j = 0; j < y.count; j++) {
            if (i + j < 35) {
                unsigned long long cur_res = ((long long) x.number[i]) * y.number[j] + uint1024_t->number[i + j];
                uint1024_t->number[i + j] = cur_res % BASE;
                if (i + j + 1 < 35) uint1024_t->number[i + j + 1] += cur_res / BASE;
            }
            else {
                printf("Undefined Behavior");
                exit(1);
            }
        }

    if (x.count == 0 || y.count == 0)
        uint1024_t->count = 1;
    else
        uint1024_t->count = x.count + y.count;

    while (uint1024_t->number[uint1024_t->count - 1] == 0 && uint1024_t->count != 1) uint1024_t->count--;

    return *uint1024_t;
}

int main() {
    Uint1024_t x;
    printf("\nfirst number: ");
    scanf_value(&x);

    Uint1024_t y;
    printf("\nsecond number: ");
    scanf_value(&y);

    Uint1024_t z1 = add_op(x, y);
    Uint1024_t z2 = subtr_op(x, y);
    Uint1024_t z3 = mult_op(x, y);

    printf("\naddition: ");
    printf_value(&z1);
    printf("\nsubtraction: ");
    printf_value(&z2);
    printf("\nmultiplication: ");
    printf_value(&z3);
    printf("\n");
    return 0;
}
