#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int binary[28];
int binary1[28];

char hex[4];

char bin_hex(int temp[]) {
    char hex = 0;
    for (int i = 0; i < 8; i++) hex += temp[i] * pow(2, (7 - i));
    return hex;
}

void bin(int binary[], int num) {
    int i = 27;
    while (num > 0) {
        binary[i] = num % 2;
        num /= 2;
        i--;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("enter more arguments");
        exit(-2);
    }

    char* tmp;
    strtok_r(argv[1], "=", &tmp);
    char* name_file = strtok_r(NULL, "", &tmp);

    FILE* file = fopen(name_file, "rb");

    if (file == NULL) {
        printf("there is no such file, choose another one (preferably not a morgenstern track)");
        exit(-1);
    }

    if (argc == 3) {
        if ((strcmp(argv[2], "--show") == 0)) {
            printf("\ninformation about your amazing file:\n\n");
            fseek(file, 6, SEEK_SET);

            int size_tag = 0, size_frame_inf = 0; //получаю размер тэга
            char temp_tag[4];
            fread(temp_tag, 1, 4, file);
            size_tag = (temp_tag[0]<<21) | (temp_tag[1]<<14) | (temp_tag[2]<<7) | (temp_tag[3]);

            int pos = 10;
            char name_frame[5], temp_frame[4], trash[3];

            while (pos <= size_tag + 10) {

                fread(name_frame, 1, 4, file);
                if ((int)name_frame[0] == 0) break;
                name_frame[4] = '\0';
                pos+=4;

                fread(temp_frame, 1, 4, file);
                pos+=4;
                size_frame_inf = (temp_frame[0]<<21) | (temp_frame[1]<<14) | (temp_frame[2]<<7) | (temp_frame[3]);
                size_frame_inf--; // байт кодировки из блока инфы

                fread(trash, 1, 3, file);
                pos+=3;

                char *info = (char *)malloc(size_frame_inf + 1);
                fread(info, 1, size_frame_inf, file);
                info[size_frame_inf] = '\0';
                pos+=size_frame_inf;
                printf("***********\n");
                puts(name_frame);
                puts(info);
                printf("***********");
                printf("\n\n");
                free(info);
            }
        }
        else {
            int f = 0;

            strtok_r(argv[2], "=", &tmp);
            char* key = strtok_r(NULL, "", &tmp);

            fseek(file, 6, SEEK_SET);

            int size_tag = 0, size_frame_inf = 0; //получаю размер тэга
            char temp_tag[4];
            fread(temp_tag, 1, 4, file);
            size_tag = (temp_tag[0]<<21) | (temp_tag[1]<<14) | (temp_tag[2]<<7) | (temp_tag[3]);

            int pos = 10;
            char name_frame[5], temp_frame[4], trash[3];

            while (pos <= size_tag + 10) {

                fread(name_frame, 1, 4, file);
                if ((int)name_frame[0] == 0) break;
                name_frame[4] = '\0';
                pos+=4;



                fread(temp_frame, 1, 4, file);
                pos+=4;
                size_frame_inf = (temp_frame[0]<<21) | (temp_frame[1]<<14) | (temp_frame[2]<<7) | (temp_frame[3]);
                size_frame_inf--; // байт кодировки из блока инфы

                fread(trash, 1, 3, file);
                pos+=3;

                char *info = (char *)malloc(size_frame_inf + 1);
                fread(info, 1, size_frame_inf, file);
                info[size_frame_inf] = '\0';
                pos+=size_frame_inf;

                if (strcmp(name_frame, key) == 0) {
                    puts(name_frame);
                    puts(info);
                    f = 1;
                }
                free(info);
            }

            if (f == 0) printf("this is fhame does not exist");
        }
    }
    else {
        strtok_r(argv[2], "=", &tmp);
        char *key = strtok_r(NULL, "", &tmp);
        strtok_r(argv[3], "=", &tmp);
        char *prop_val = strtok_r(NULL, "", &tmp);
        int len = strlen(prop_val);

        FILE *ptrFile = fopen("temp.mp3", "wb");

        char buffer[6]; // переписала 6 байт до размера
        fread(buffer, 1, 6, file);
        fwrite(buffer , 1 , sizeof(buffer) , ptrFile);

        int size_tag = 0, size_frame_inf = 0; //получаю размер тэга
        char temp_tag[4];
        fread(temp_tag, 1, 4, file);
        size_tag = (temp_tag[0]<<21) | (temp_tag[1]<<14) | (temp_tag[2]<<7) | (temp_tag[3]);

        int pos = 10;
        int final_pos, old_inf_pos;
        int flag = 0;
        char name_frame[5], temp_frame[4], trash[3];

        while (pos <= size_tag + 10 && (flag == 0)) {

            fread(name_frame, 1, 4, file);
            if ((int)name_frame[0] == 0) break;
            name_frame[4] = '\0';

            if (strcmp(name_frame, key) == 0) {
                final_pos = pos;
                flag = 1;
            }
            pos+=4;

            fread(temp_frame, 1, 4, file);
            pos+=4;
            size_frame_inf = (temp_frame[0]<<21) | (temp_frame[1]<<14) | (temp_frame[2]<<7) | (temp_frame[3]);
            size_frame_inf--; // байт кодировки из блока инфы

            fread(trash, 1, 3, file);
            pos+=3;

            char *info = (char *)malloc(size_frame_inf);
            fread(info, 1, size_frame_inf, file);
            pos+=size_frame_inf;
            free(info);

            if (flag == 1) old_inf_pos = pos;
        }

        if (flag == 0) {
            printf("frame = prop_name does not exist");
            exit(2);
        }

        fseek(file, 6, SEEK_SET);
        // записываю новый размер тега, переводу его сначала в двойчный, потом в синсэйф, потом в байты
        int size_tag2 = size_tag;
        size_tag2 -= size_frame_inf - len;
        bin(binary, size_tag2);
        int temp[32];

        for (int i = 0, j = 0; i < 32; i++) {
            if (i == 0 || i == 8 || i == 16 || i == 24) temp[i] = 0;
            else temp[i] = binary[j++];
        }

        int temp8[8];

        for (int i = 0; i < 8; i++) temp8[i] = temp[i];
        char c0 = bin_hex(temp8);
        for (int i = 0; i < 8; i++) temp8[i] = temp[i + 8];
        char c1 = bin_hex(temp8);
        for (int i = 0; i < 8; i++) temp8[i] = temp[i + 16];
        char c2 = bin_hex(temp8);
        for (int i = 0; i < 8; i++) temp8[i] = temp[i + 24];
        char c3 = bin_hex(temp8);

        char c[4] = {c0, c1, c2, c3};
        fwrite(c , 1 , sizeof(c) , ptrFile);

        int pos1 = 10;
        fseek(file, 10, SEEK_SET);
        while (pos1 <= size_tag + 10) {

            if (pos1 == final_pos) {
                fread(name_frame, 1, 4, file);
                pos1+=4;
                fwrite(name_frame, 1 , 4 , ptrFile);

                fread(temp_frame, 1, 4, file);
                pos1+=4;
                size_frame_inf = (temp_frame[0]<<21) | (temp_frame[1]<<14) | (temp_frame[2]<<7) | (temp_frame[3]);
                int size_frame_inf2 = size_frame_inf;
                size_frame_inf2 -= size_frame_inf - (len + 1); //len не учитывает байт кодировки
                bin(binary1, size_frame_inf2);

                for (int i = 0, j = 0; i < 32; i++) {
                    if (i == 0 || i == 8 || i == 16 || i == 24) temp[i] = 0;
                    else temp[i] = binary1[j++];
                }

                for (int i = 0; i < 8; i++) temp8[i] = temp[i];
                c0 = bin_hex(temp8);
                for (int i = 0; i < 8; i++) temp8[i] = temp[i + 8];
                c1 = bin_hex(temp8);
                for (int i = 0; i < 8; i++) temp8[i] = temp[i + 16];
                c2 = bin_hex(temp8);
                for (int i = 0; i < 8; i++) temp8[i] = temp[i + 24];
                c3 = bin_hex(temp8);

//            c2='u'; c3='f';
                char c_1[4] = {c0, c1, c2, c3};

                fwrite(c_1 , 1 , 4 , ptrFile);

                fread(trash, 1, 3, file);
                fwrite(trash , 1 , 3 , ptrFile);
                pos1 += 3;

                fwrite(prop_val, 1, len, ptrFile);
                pos1+=(size_frame_inf - 1);
                fseek(file, old_inf_pos, SEEK_SET);

            }
            else {
                fread(name_frame, 1, 4, file);
                fwrite(name_frame, 1, 4, ptrFile);
                if ((int) name_frame[0] == 0) break;
                pos1 += 4;

                fread(temp_frame, 1, 4, file);
                pos1 += 4;
                fwrite(temp_frame, 1, 4, ptrFile);
                size_frame_inf = (temp_frame[0] << 21) | (temp_frame[1] << 14) | (temp_frame[2] << 7) | (temp_frame[3]);
                size_frame_inf--; // байт кодировки из блока инфы

                fread(trash, 1, 3, file);
                fwrite(trash, 1, 3, ptrFile);
                pos1 += 3;

                char *info = (char *) malloc(size_frame_inf);
                fread(info, 1, size_frame_inf, file);
                fwrite(info, 1, size_frame_inf, ptrFile);
                pos1 += size_frame_inf;
                free(info);
            }
        }
        int new_symbol;
        while ((new_symbol = fgetc(file)) != EOF) fputc(new_symbol, ptrFile);

        fclose(file);
        fclose(ptrFile);
        remove(name_file);
        rename("temp.mp3", name_file);


    }
    return 0;
}
