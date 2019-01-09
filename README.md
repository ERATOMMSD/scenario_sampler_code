# scenario_sampler_code
This is an XML-oriented test scenario sampler.

## Installation
1. `make`
2. (optional) install [PICT] (https://github.com/Microsoft/pict)

## Usage
`./sss [options] [input]`

## Input Format
Input is a text mixed with XML elements.

### Processed Elements
- `<repeat>`:
  repetition of the content.
  Attributes `minOccurs` and `maxOccurs` specify how often it can be repeated.
  Default `minOccurs` is 0. If `maxOccurs` is omitted, currently `minOccurs` plus 10 is set.
  Attribute `delim` specifies a deliminator, which will be put in between the repetitions.
- `<random>`: random real number. Attributes `min` and `max` specify the range.
- `<choice>`: discrete choice from the child nodes.
- `<text>`: Text content.

## Options
- `-c`: resolve `<choice>` using external combinatorial test tool (defalt: PICT).

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
