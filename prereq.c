#include "prereq.h"

// 선행 과목 추가 함수
int add_prereq(Subject* child, Subject* parent) {
    // 이미 선행 과목 관계가 존재하는지 확인
    for (int i = 0; i < child->parent_count; i++) {
        if (child->parents[i] == parent) {
            return 1; // 이미 존재하는 관계
        }
    }

    child->parent_count++;
    child->parents = (Subject**)realloc(child->parents, child->parent_count * sizeof(Subject*));
    child->parents[child->parent_count - 1] = parent;

    parent->child_count++;
    parent->childs = (Subject**)realloc(parent->childs, parent->child_count * sizeof(Subject*));
    parent->childs[parent->child_count - 1] = child;

    // 사이클 발생 여부 확인
    if (check_cycle(parent)) {
        // 추가한 관계를 원복
        child->parent_count--;
        parent->child_count--;

        // 부모에서 마지막 추가된 자식 제거
        child->parents = (Subject**)realloc(child->parents, child->parent_count * sizeof(Subject*));
        parent->childs = (Subject**)realloc(parent->childs, parent->child_count * sizeof(Subject*));

        return 2; // 사이클 발생
    }

    return 0; // 성공적으로 추가됨
}

// 선행 과목 제거 함수
int remove_prereq(Subject* child, Subject* parent) {
    int parent_found = 0;
    int child_found = 0;

    // 부모에서 자식 제거
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->childs[i] == child) {
            parent_found = 1;
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->childs[j] = parent->childs[j + 1];
            }
            parent->child_count--;
            parent->childs = (Subject**)realloc(parent->childs, parent->child_count * sizeof(Subject*));
            break;
        }
    }

    // 자식에서 부모 제거
    for (int i = 0; i < child->parent_count; i++) {
        if (child->parents[i] == parent) {
            child_found = 1;
            for (int j = i; j < child->parent_count - 1; j++) {
                child->parents[j] = child->parents[j + 1];
            }
            child->parent_count--;
            child->parents = (Subject**)realloc(child->parents, child->parent_count * sizeof(Subject*));
            break;
        }
    }

    return (parent_found && child_found) ? 0 : -1; // 성공적으로 제거됨
}



// 큐 정의
typedef struct Queue {
    Subject **subjects;
    int front;
    int rear;
    int capacity;
} Queue;

// 큐 초기화 함수
Queue* create_queue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->subjects = (Subject**)malloc(capacity * sizeof(Subject*));
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = capacity;
    return queue;
}

// 큐 삽입 함수
void enqueue(Queue* queue, Subject* subject) {
    if (queue->rear == queue->capacity) {
        queue->capacity *= 2;
        queue->subjects = (Subject**)realloc(queue->subjects, queue->capacity * sizeof(Subject*));
    }
    queue->subjects[queue->rear++] = subject;
}

// 큐 삭제 함수
Subject* dequeue(Queue* queue) {
    if (queue->front == queue->rear) {
        return NULL;
    }
    return queue->subjects[queue->front++];
}

// 큐가 비어있는지 확인하는 함수
int is_empty(Queue* queue) {
    return queue->front == queue->rear;
}

// 사이클 감지 함수
int check_cycle(Subject* subject) {
    Queue* queue = create_queue(10); // 큐 초기화
    enqueue(queue, subject); // 초기 과목을 큐에 추가

    // 방문 여부를 추적하기 위한 해시 테이블 크기
    int visited_size = 100;
    Subject** visited = (Subject**)calloc(visited_size, sizeof(Subject*));

    while (!is_empty(queue)) {
        Subject* current = dequeue(queue); // 큐에서 과목을 하나 꺼냄

        // 이미 방문한 노드인지 확인
        for (int i = 0; i < visited_size; i++) {
            if (visited[i] == current) { // 현재 과목이 이미 방문한 과목이라면
                if (current == subject) { // 그리고 그 과목이 시작 과목이라면
                    free(queue->subjects); // 메모리 해제
                    free(queue);
                    free(visited);
                    return 1; // 사이클이 발견됨
                }
                break;
            }
        }

        // 방문하지 않은 노드라면 방문 표시
        for (int i = 0; i < visited_size; i++) {
            if (visited[i] == NULL) {
                visited[i] = current; // 방문한 과목 배열에 추가
                break;
            }
        }

        // 부모 과목을 큐에 추가
        for (int i = 0; i < current->parent_count; i++) {
            enqueue(queue, current->parents[i]);
        }
    }

    free(queue->subjects); // 메모리 해제
    free(queue);
    free(visited);
    return 0; // 사이클이 발견되지 않음
}