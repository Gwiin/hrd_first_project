# node_b notes

- node_c integration handoff package
- current contract uses `house/cmd/light`, `house/cmd/window`, `house/status/nodeB`, `house/heartbeat/nodeB`
- actuator node role only, not independent sensor publishing
- hardware pin mapping summary
- `GP14` servo
- `GP16` WS2812 RGB Strip data
- current WS2812 behavior: 8 pixels all white on `lamp=ON`, all off on `lamp=OFF`
- latest upload file: [node_b.uf2](/home/asd/hrd_first_project/node_b/build/node_b.uf2)
