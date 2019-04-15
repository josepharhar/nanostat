#!/usr/bin/env node

const fs = require('fs');
const assert = require('assert').strict;
const util = require('util');

const nanostat = require('nanostat');

BigInt.prototype.toJSON = function() {
  return this.toString();
}

function pad(num, numPadding) {
  let str = num.toString();
  while (str.length < numPadding) {
    str = '0' + str;
  }
  return str;
}

function dateToString(date) {
  return date.getFullYear()
    + '-' + pad((date.getMonth() + 1), 2)
    + '-' + pad(date.getDate(), 2)
    + ' ' + pad(date.getHours(), 2)
    + ':' + pad(date.getMinutes(), 2)
    + ':' + pad(date.getSeconds(), 2);
}

function tzOffsetToString(date) {
  const addMinus = date.getTimezoneOffset() >= 0;
  const absTzOffset = Math.abs(date.getTimezoneOffset());
  return (addMinus ? '-' : '') + pad(Math.abs(date.getTimezoneOffset()) / 60 * 100, 4);
}

if (process.argv.length !== 3) {
  console.log('usage: node jstat.js filepath');
  return;
}
const filepath = process.argv[2];

const stat = nanostat.statSync(filepath);

console.log(`  File: ${filepath}`);
console.log(`  Size: ${stat.size}`);

const aDate = new Date(Number(stat.atimeMs));
const mDate = new Date(Number(stat.mtimeMs));
const cDate = new Date(Number(stat.ctimeMs));

console.log(`Access: ${dateToString(aDate)}.${stat.atimeNs} ${tzOffsetToString(aDate)}`);
console.log(`Modify: ${dateToString(mDate)}.${stat.mtimeNs} ${tzOffsetToString(mDate)}`);
console.log(`Create: ${dateToString(cDate)}.${stat.ctimeNs} ${tzOffsetToString(cDate)}`);
