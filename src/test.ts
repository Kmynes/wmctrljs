import {
    getWindowList,
    getActiveWindow,
    getWindowsByPid,
    getWindowsByClassName,
    activeWindowById,
    activeWindowsByPid,
    activeWindowsByClassName,
    closeWindowById,
    closeWindowsByPid,
    closeWindowsByClassName,
    windowMoveResize
} from "./index";
import { promisify } from "util";
import * as cp from "child_process";

closeWindowById(4);

const winProcess = "xclock";
const class_name = "xclock.XClock";

const exec = promisify(cp.exec);

function wait(time:number):Promise<void> {
    return new Promise((resolve) => {
        setTimeout(() => { resolve(); }, time * 1000);
    });
}

(async () => {
    console.time("getWindowList");
    getWindowList();
    console.timeEnd("getWindowList");
    try {
        closeWindowsByClassName(class_name);
    }catch(e) {
        //Voluntary error [Must failed]
        console.error("Voluntary error [Must failed]", e);
    }

    for (let i = 0; i < 25; i++)
        exec(winProcess);
    await wait(3);

    console.time("getActiveWindow");
    getActiveWindow();
    console.timeEnd("getActiveWindow");

    console.time("activeWindowsByClassName");
    activeWindowsByClassName(class_name);
    console.timeEnd("activeWindowsByClassName");
    await wait(1);

    const xclockWindows = getWindowsByClassName(class_name);
    if (xclockWindows) {
        const winXclock = xclockWindows[0];
        console.time("getWindowsByPid");
        getWindowsByPid(winXclock.win_pid);
        console.timeEnd("getWindowsByPid");
        await wait(1);

        console.time("activeWindowById");
        activeWindowById(winXclock.win_id);
        console.timeEnd("activeWindowById");
        await wait(1);

        console.time("activeWindowsByPid");
        activeWindowsByPid(winXclock.win_pid);
        console.timeEnd("activeWindowsByPid");
        await wait(1);

        for (let i = 0; i < xclockWindows.length; i++) {
            const win = xclockWindows[i];
            if (i % 2 === 0) {
                console.time("closeWindowById");
                closeWindowById(win.win_id);
                console.timeEnd("closeWindowById");
            }
            else {
                console.time("closeWindowsByPid");
                closeWindowsByPid(win.win_pid);
                console.timeEnd("closeWindowsByPid");
            }
            await wait(0.1);
        }

        try {
            closeWindowsByClassName(class_name);
        }
        catch(e) {
            //Voluntary error [Must failed]
            console.error("Voluntary error [Must failed]", e);
        }

        exec("xeyes");
        await wait(2);

        console.time("activeWindowsByClassName");
        activeWindowsByClassName("xeyes.XEyes");
        console.timeEnd("activeWindowsByClassName");

        try {
            getActiveWindow();
        } catch(e) {
            //Voluntary error [Must failed]
            console.error("Voluntary error [Must failed]", e);
        }

        console.time('getWindowsByClassName');
        const winsXeyes = getWindowsByClassName("xeyes.XEyes");
        console.timeEnd('getWindowsByClassName');

        if (!winsXeyes || !winsXeyes.length)
            throw new Error("Unexpected empty array");

        console.time("windowMoveResize");
        windowMoveResize(winsXeyes[0].win_id, 0, 0, 0, 500, 500);
        console.timeEnd("windowMoveResize");
        await wait(2);

        console.time("windowMoveResize");
        windowMoveResize(winsXeyes[0].win_id, 0, 0, 0, 1000, 1000);
        console.timeEnd("windowMoveResize");
        await wait(5);

        console.time("closeWindowsByClassName");
        closeWindowsByClassName("xeyes.XEyes");
        console.timeEnd("closeWindowsByClassName");
    }
})().catch(console.error);