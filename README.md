# ssimulaCraPP
A C++ port of https://codeberg.org/Kosaka/ssimulacrapy that takes vapoursynth scripts as input

# Dependencies

- Vapoursynth
- [Vship](https://github.com/Line-fr/Vship) (optional)
- [vszip](https://github.com/dnjulek/vapoursynth-zip) (optional)
- [julek-plugin](https://github.com/dnjulek/vapoursynth-julek-plugin) (optional)

One Importer at least

- [ffms2](https://github.com/FFMS/ffms2/tree/master)
- [bestsource](https://github.com/vapoursynth/bestsource)
- [LSMASH](https://github.com/l-smash/l-smash)

- [Python] (optional)

# Compilation

To install with support for vapoursynth script inputs (requires python)

`make build`

To install without support for vapoursynth script inputs

`make buildNoScript`
# Usage

usage: ./ssimulaCraPP [-h] [--source SOURCE] [--encoded ENCODED] [--sourceScript SCRIPT OUTPUTNODE] [--encodedScript SCRIPT OUTPUTNODE]
                    [-i {ffms2,bestsource,lsmash}] [-si {ffms2,bestsource,lsmash}]
                    [-ei {ffms2,bestsource,lsmash}]
                    [-m {ssimu2_vszip,ssimu2_vship,butter_vship,ssimu2_jxl,butter_jxl}]
                    [-t THREADS] [--width WIDTH] [--height HEIGHT] [-e EVERY] [--start START] [--end END]
                    [--installed] [--version] [-v]

Use metrics to score the quality of videos compared to a source.

options:
  -h, --help            show this help message and exit
  --source SOURCE       Source video path. Can be relative to this script or a full path.
  --encoded ENCODED
                        Encoded video path. Can be relative to this script or a full path.
  -i, --importer {ffms2,bestsource,lsmash}
                        Video importer for the source and encoded videos.
  -si, --source-importer {dgdecnv,ffms2,bestsource,lsmash}
                        Source video importer. Overrides -i (--importer) for the source video.
  -ei, --encoded-importer {dgdecnv,ffms2,bestsource,lsmash}
                        Encoded video importer. Overrides -i (--importer) for the encoded video.
  -m, --metric {ssimu2_vszip,ssimu2_vship,butter_vship,ssimu2_jxl,butter_jxl}
                        Metric to use. (default: ssimu2_vszip)
  -t, --threads THREADS
                        Number of threads to use. Default: -1 (Auto).
  -g, --gpu-threads THREADS
                        Number of gpu threads to use for vship. Default: 8.
  -e, --every EVERY     Frames calculated every nth frame. Default: 1 (Every frame is calculated). For example,
                        setting this to 5 will calculate every 5th frame.
  --start START         Start frame. Default: 0 (First frame).
  --end END             End frame. Default: (Last frame).
  --installed           Print out a list of dependencies and whether they are installed then exit.
