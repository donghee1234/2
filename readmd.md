# 과제 10: 정렬 알고리즘의 성능 및 안정성 비교

본 과제는 0부터 1,000,000 사이의 정수 2개(Key 1, Key 2)로 구성된 10,000개의 데이터를 생성하고, **선택 정렬(Selection Sort), 버블 정렬(Bubble Sort), 조기 종료 버블 정렬(Bubble Sort with Early Termination), 삽입 정렬(Insertion Sort)**의 성능(비교 횟수 및 데이터 이동 횟수)과 안정성(Stability)을 비교·분석하는 프로그램입니다.

---

## 1. 프로젝트 개요 및 실험 방법

- **데이터 규모**: 10,000개
- **키 구성**: 
  - 첫 번째 키 (`Key 1`): 0 ~ 1,000,000 사이의 임의의 정수 (오름차순 정렬 기준)
  - 두 번째 키 (`Key 2`): 0 ~ 1,000,000 사이의 임의의 정수 (동일한 Key 1을 가진 데이터의 상대적 순서 확인용)
- **무작위 데이터 100회 실험**: 동일한 무작위 데이터 세트를 100회 반복 생성하여 각 알고리즘의 총 비교 횟수 및 평균 비교/이동 횟수를 측정하였습니다.
- **초기 정렬 상태별 실험**: 무작위 데이터 외에 `이미 정렬된 데이터`, `역순으로 정렬된 데이터`, `일부 정렬된 데이터` 등 4가지 입력 상태에 따른 성능 변화를 측정하였습니다.

---

## 2. 무작위 데이터 100회 실험 결과

(실행 결과 출력된 수치를 여기에 작성하세요. 예시 형식)

- **Selection Sort**
  - Total Comparisons : X
  - Average Comparisons : X
  - Average Movements : X
- **Bubble Sort**
  - Total Comparisons : X
  - Average Comparisons : X
  - Average Movements : X
- **Bubble Sort (Early Termination)**
  - Total Comparisons : X
  - Average Comparisons : X
  - Average Movements : X
- **Insertion Sort**
  - Total Comparisons : X
  - Average Comparisons : X
  - Average Movements : X

---

## 3. 일반 버블 정렬과 조기 종료 버블 정렬의 비교

- **조기 종료 조건(Early Termination)**: 한 번의 순회(Pass) 동안 인접한 데이터의 교환이 단 한 번도 발생하지 않았다면, 이미 모든 데이터가 정렬된 것으로 판단하고 정렬을 즉시 종료합니다.
- **분석
