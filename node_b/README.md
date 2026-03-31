# node_b

Pico 2 W + Tower Pro SG90 서보 테스트용 프로젝트입니다.

배선:

- Pico `GPIO16` -> SG90 `signal`
- Pico `GND` -> SG90 `brown`
- USB 5V 또는 외부 5V -> SG90 `red`

주의:

- SG90는 순간 전류가 커서 USB 전원만으로도 테스트는 가능하지만 떨림이 생길 수 있습니다.
- 더 안정적으로 돌리려면 외부 5V 전원을 쓰고, Pico와 서보의 `GND`는 반드시 공통으로 연결하세요.
- Pico GPIO에는 서보 전원선을 직접 연결하지 말고, 신호선만 GPIO에 연결하세요.

현재 동작:

- `0 -> 45 -> 90 -> 135 -> 180 -> 90` 순서로 1.5초 간격 이동
- PWM 50Hz, GPIO16 사용

빌드 예시:

```bash
cd node_b
mkdir -p build
cd build
cmake ..
make -j4
```

생성된 `node_b.uf2`를 Pico 2 W에 복사하면 됩니다.
