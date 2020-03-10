{
  "targets": [
    {
      "target_name": "alarm_clock",
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "sources": [ "lib/alarm_clock.cpp", "lib/timer.cpp" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "conditions": [
        ["OS=='linux'", {
          "ldflags": [ "-pthread" ],
          "libraries": [ "-lrt" ]
        }]
      ]
    }
  ]
}
