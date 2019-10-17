# scenario_sampler_code
This is an XML-oriented test scenario sampler.

## Installation
1. `make`
2. (optional) install [PICT] (https://github.com/Microsoft/pict)

## Usage
`./sss [options] [input]`

## Options
- `-c`: resolve `<choice>` using external combinatorial test tool (defalt: PICT).
- `-r:<n>`: specifies how many l2-scripts an l3-script expands to (default: 1).
- `-u:<n>`: specifies how many l0-scripts an l1-script expands to (default: 1).

## Input Format
Input is a text mixed with XML elements: an l3-script.

### Processed Elements
- `<repeat>`:
  repetition of the content. Constitutes an l3-script.
  Attributes `minOccurs` and `maxOccurs` specify how often it can be repeated.
  Default `minOccurs` is 0. If `maxOccurs` is omitted, currently `minOccurs` plus 10 is set.
  Attribute `delim` specifies a deliminator, which will be put in between the repetitions.
- `<choice>`: discrete choice from the child nodes. Constitutes an l2-script.
- `<random>`:
  random real number. Constitutes an l1-script.
  Attributes `min` and `max` specify the range.
- `<text>`: Text content.

## Examples

By `./sss samples/lr.xml`, where `lr.xml` is
```xml
this is a random walk scenario.
<repeat>Go <choice>
             <text>left</text>
             <text>right</text>
           </choice>.
</repeat>
```
the output will be, for instance,

```
this is a random walk scenario.
Go right.
Go left.
Go left.
Go right.
Go left.
Go right.
Go left.
```

## License
<a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-nd/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/">Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License</a>.
