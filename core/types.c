#include "types.h"
#include <stdlib.h>

void result_free(Result* r) {
    // Пользователь должен явно освобождать ok/err, если это динамические данные
    // Здесь можно добавить кастомную логику, если есть поле type в Generic
    // Пример:
    // if (r->is_ok && r->ok.value) free(r->ok.value);
    // else if (!r->is_ok && r->err.value) free(r->err.value);
    // По умолчанию ничего не делаем
}

void function_params_free(FunctionParams* p) {
    if (p && p->argv) {
        // Если argv был выделен через malloc
        free(p->argv);
        p->argv = NULL;
    }
}

void function_pipeline_free(FunctionPipeline* fp) {
    while (fp) {
        FunctionPipeline* next = fp->next;
        free(fp);
        fp = next;
    }
}

int function_pipeline_validate(FunctionPipeline* head) {
    // Проверка на циклы (алгоритм Флойда)
    FunctionPipeline* slow = head;
    FunctionPipeline* fast = head;
    while (fast && fast->next) {
        if (!slow->func) return 0;
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast && slow != NULL) return 0; // цикл
    }
    // Проверка, что все func != NULL
    while (head) {
        if (!head->func) return 0;
        head = head->next;
    }
    return 1;
} 