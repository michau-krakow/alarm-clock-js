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

describe('AlarmClock.setAlarm', function () {
  it('should throw when called with wrong arguments', function () {
    const delay = 1;
    const cback = () => {};

    expect(() => aClock.setAlarm()).to.throw(Error, /number of arguments/);
    expect(() => aClock.setAlarm(delay)).to.throw(Error, /number of arguments/);
    expect(() => aClock.setAlarm(cback)).to.throw(Error, /number of arguments/);
    expect(() => aClock.setAlarm(delay, delay)).to.throw(TypeError, /should be a function/);
    expect(() => aClock.setAlarm(cback, cback)).to.throw(TypeError, /should be a date or number/);
    expect(() => aClock.setAlarm(cback, 0)).to.throw(Error, /should be greater than zero/);
    expect(() => aClock.setAlarm(cback, -1)).to.throw(Error, /should be greater than zero/);
  });

  it('should be able to schedule alarm after given delay', function (done) {
    this.timeout(200);
    expect(aClock.setAlarm(done, 100));
  });

  it('should return cancellation function', function () {
    expect(aClock.setAlarm(()=>{}, 10)).to.be.a('function');
  });

  it('should be able to cancel running alarm', function (done) {
    setTimeout(done, 200); // success, unless the timer fires earlier...

    const cancelFn = aClock.setAlarm(() => done('Cancelled alarm should not fire!'), 100);
    expect(cancelFn).to.be.a('function');
    cancelFn();
  });

  it('should reject past date given as alarm time', function () {
    const past = new Date(Date.now() - 10);

    // make sure argument is a Date object
    expect(past).to.be.a('Date');
    expect(past.getTime()).to.be.lessThan(Date.now());

    const nop = () => {};
    expect(() => aClock.setAlarm(nop, past)).to.throw(Error, /cannot be in the past/);
  });

  it('should be able to schedule alarm at future Date', function (done) {
    this.timeout(100);
    const in1sec = new Date(Date.now() + 75);

    // make sure argument is a Date object
    expect(in1sec).to.be.a('Date');
    expect(in1sec.getTime()).to.be.within(Date.now(), Date.now()+100);

    expect(aClock.setAlarm(done, in1sec)).to.be.a('function');
  });

  // it('should throw when scheduling alarm in the past', function () {
  // });
  // it('should not throw when cancelling old alarm', function () {
  // });
  // it('should not throw when cancelling alarm again', function () {
  // });
  // it('should not throw when cancelling alarm again', function () {
  // });
  // should be able to create more independent timers
});
