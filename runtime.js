let canvas = document.getElementById("canvas");
let ctx = canvas.getContext("2d");
var simulationSpaceSize;
let ready = false;
var lastXMousePos = 0;
var lastYMousePos = 0;

Module.onRuntimeInitialized = () => {
    ready = true;
    simulationSpaceSize = Module.__Z9JSGetSidei();
    tick()
}
let isDraging = false;

canvas.addEventListener('mousedown', e => {
    isDraging = true;
    lastXMousePos = e.offsetX;
    lastYMousePos = e.offsetY;
});

canvas.addEventListener('mouseup', e => {
    isDraging = false;
    Module.__Z20JSRefreshMouseVectordddd(0, 0, 0, 0)
});

function min(a, b) {
    return a < b ? a : b;
}

canvas.addEventListener('mousemove', e => {
    if (ready && isDraging) {
        let x = e.offsetX;
        let y = e.offsetY;
        let maxForce = 0.2;
        let vectorMultiplier = 0.01;
        Module.__Z20JSRefreshMouseVectordddd(e.offsetX / Math.min(canvas.clientHeight, canvas.clientWidth), e.offsetY / Math.min(canvas.clientHeight, canvas.clientWidth), min(vectorMultiplier * (x - lastXMousePos), maxForce), min(maxForce, vectorMultiplier * (y - lastYMousePos)))
        lastXMousePos = x;
        lastYMousePos = y;
    }


});

function tick() {
    canvas.height = canvas.clientHeight;
    canvas.width = canvas.clientWidth;
    ctx.fillStyle = "rgba(255, 255, 255,1)";
    ctx.fillRect(0, 0, 10000, 10000);
    ctx.fillStyle = "rgba(0,0,0,1)";
    Module.__Z6JSStepd(1)
    for (let i = 0; i < Module.__Z18JSGetParticleCountv(); i++) {

        drawPoint(__Z6JSGetXi(i), __Z6JSGetYi(i))
    }
    console.log("tick");
    setTimeout(tick, 1);
}


function drawPoint(x, y) {

    let ScreenX = x * Math.min(canvas.clientHeight, canvas.clientWidth) / simulationSpaceSize;
    let ScreenY = y * Math.min(canvas.clientHeight, canvas.clientWidth) / simulationSpaceSize;

    ctx.fillRect(ScreenX, ScreenY, 2, 2);
}