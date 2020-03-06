const { expect } = require('chai');

const aClock = (() => {
  try       { return require('../build/Release/alarm_clock'); }
  catch (e) { return require('../build/Debug/alarm_clock'); }
})();

describe('AlarmClock', function() {
  it('should be an object', function () {
    expect(aClock).to.be.an('Object');
  });

  it('should have required properties', function () {
    expect(aClock).to.have.property('setImmediate').which.is.a('function');
    expect(aClock).to.have.property('setAlarm').which.is.a('function');
  });
});

describe('AlarmClock.setImmediate', function () {
  it('should throw when called with wrong arguments', function () {
    const delay = 1;

    expect(() => aClock.setImmediate()).to.throw(Error, /number of arguments/);
    expect(() => aClock.setImmediate(delay)).to.throw(TypeError, /should be a function/);
  });

  it('should immediately call callback', function () {
    var called = false;
    const cb = () => called = true;

    expect(aClock.setImmediate(cb)).to.be.equal(undefined);
    expect(called).to.be.equal(true);
  });
});

