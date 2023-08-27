# [pico_pio_buzz](https://github.com/benevpi/pico_pio_buzz/blob/)  

A simple PIO program that outputs a tone based on a number going into it. I've not yet done the maths to work out what number means what note, but it seems to give a nice range over audible tones. A bit buzzy at lower frequencies, but sounds nicer higher up.

**usage**：

```python
>>> import pio_beep
>>> beeper = pio_beep.beep(0,0)
>>> beeper.play_pitch(10,1,400)
```

**demo**：

played happy birthday

## values vs. Pitchs

The PIO program plays a 'value' which is a number between 0 and 5000 (adjustable if you want to). You can either precalculate these using the calc_value(hertz) method, and then play them using play_value, or you can calculate them on the fly using play_pitch which takes an argument in Hz. The latter adds a little more on-the-hoof overhead, but it shouldn't be too significant unless you're trying to get super-fine timings.

**play_value(note_len, pause_len, value)** plays the value for note_len seconds followed by a pause of pause_len
**play_pitch(note_len, pause_len, hertz)** plays the pitch hertz for note_len followed by a pause of pause_len