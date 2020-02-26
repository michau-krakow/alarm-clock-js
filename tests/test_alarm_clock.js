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

