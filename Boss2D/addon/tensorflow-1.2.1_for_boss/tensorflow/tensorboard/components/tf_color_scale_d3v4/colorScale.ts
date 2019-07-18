/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// Example usage:
// runs = ["train", "test", "test1", "test2"]
// ccs = new ColorScale();
// ccs.domain(runs);
// ccs.getColor("train");
// ccs.getColor("test1");
import * as d3 from 'd3';  // from //third_party/javascript/typings/d3_v4
import {palettes} from './palettes'


export class ColorScale {
  private palette: string[];
  private identifiers = d3.map();

  /**
   * Creates a color scale with optional custom palette.
   *  @param {string[]} [palette=palettes.googleColorBlind] - The color
   *                 palette you want as an Array of hex strings.
   */
  constructor(palette: string[] = palettes.googleColorBlindAssist) {
    this.palette = palette;
  }

  /**
   * Set the domain of strings.
   * @param {string[]} strings - An array of possible strings to use as the
   *                             domain for your scale.
   */
  public domain(strings: string[]): this {
    this.identifiers = d3.map();
    strings.forEach((s, i) => {
      this.identifiers.set(s, this.palette[i % this.palette.length]);
    });
    return this;
  }

  /**
   * Use the color scale to transform an element in the domain into a color.
   * @param {string} The input string to map to a color.
   * @return {string} The color corresponding to that input string.
   * @throws Will error if input string is not in the scale's domain.
   */
  public scale(s: string): string {
    if (!this.identifiers.has(s)) {
      throw new Error('String was not in the domain.');
    }
    return this.identifiers.get(s) as string;
  }
}

Polymer({
  is: 'tf-color-scale',
  properties: {
    runs: {
      type: Array,
    },
    outColorScale: {
      type: Object,
      readOnly: true,
      notify: true,
      value: function() {
        return new ColorScale();
      },
    },
  },
  observers: ['updateColorScale(runs.*)'],
  updateColorScale: function(runsChange) {
    this.outColorScale.domain(this.runs);
  },
});
