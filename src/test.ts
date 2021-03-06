import {
    getScreenSync,
    getWindowListSync,
    getActiveWindowSync,
    getWindowsByPidSync,
    getWindowsByClassNameSync,
    activeWindowByIdSync,
    activeWindowsByPidSync,
    activeWindowsByClassNameSync,
    closeWindowByIdSync,
    closeWindowsByPidSync,
    closeWindowsByClassNameSync,
    windowMoveResizeSync,
    windowStateSync,
    windowMinimize,
    Window,
    getWindowsByClassName,
    getWindowList,
    windowAllowAllSizesSync,
    windowRaiseSync,
    windowMinimizeSync
} from "./index";

import { promisify } from "util";
import * as cp from "child_process";
import { wait } from "./unit-test/utils";

const winProcess = "xclock";
const class_name = "xclock.XClock";

const exec = promisify(cp.exec);

(async () => {
    for(let i = 0; i < 100; i++) {
        console.time("closeWindowById");
        closeWindowByIdSync(4);
        console.timeEnd("closeWindowById");
    }
 
    for(let i = 0; i < 100; i++) {
        console.time("getScreenSync");
        const screen = getScreenSync();
        console.timeEnd("getScreenSync");
        console.log(screen);
    }

    let windows = [] as Window[];
    for (let i = 0; i < 100; i++) {
        console.time("getWindowListSync");
        windows = await getWindowList();
        console.timeEnd("getWindowListSync");
    }

    for(let i = 0; i < 100; i++) {
        console.time("getWindowListSync");
        windows = getWindowListSync();
        console.timeEnd("getWindowListSync");
    }

    const frames = windows.filter(win => win.frame_extents)
            .map(({frame_extents}) => {
                if (frame_extents)
                    return `left:${frame_extents.left},right:${frame_extents.right},top:${frame_extents.top},bottom:${frame_extents.bottom},`;
            })
            .join('\n');
    console.log(frames);

    for(let i = 0; i < 100; i++) {    
        try {
            closeWindowsByClassNameSync(class_name);
        }catch(e) {
            //Voluntary error [Must failed]
            console.error("Voluntary error [Must failed]", e.message);
        }
    }

    for (let i = 0; i < 50; i++) {
        exec(winProcess);
        await wait(0.1);
    }
    await wait(5);

    console.time("getWindowsByClassName");
    const xclockWindows = getWindowsByClassNameSync(class_name);
    console.timeEnd("getWindowsByClassName");
    if (!xclockWindows)
        throw("Unexpected error xclockWinsows");

    console.log(`${xclockWindows.length} windows are open with the class_name ${class_name}`);

    try {
        console.time("activeWindowsByClassName");
        activeWindowsByClassNameSync(class_name);
        console.timeEnd("activeWindowsByClassName");
    } catch (e) {
        console.log(`Voluntary error [Must failed][activeWindowsByClassName] ${e.message}`);
        getWindowsByClassNameSync(class_name)
            .forEach(win => activeWindowByIdSync(win.win_id));
    }
    await wait(1);

    for (const winXclock of xclockWindows) {
        console.time("closeWindowsByPid");
        closeWindowsByPidSync(winXclock.win_pid);
        console.timeEnd("closeWindowsByPid");
        await wait(0.1);
    }
    await wait(1);

    try {
        closeWindowsByClassNameSync(class_name);
    }
    catch(e) {
        //Voluntary error [Must failed]
        console.error("Voluntary error [Must failed]", e.message);
    }
    await wait(1);

    for (let i = 0; i < 100; i++) {
        try {
            console.time("getActiveWindow");
            getActiveWindowSync();
            console.timeEnd("getActiveWindow");
        } catch (e) {
            //Voluntary error [Must failed]
            console.error("Voluntary error [Must failed]", e.message);
            console.timeEnd("getActiveWindow");  
        }
    }

    exec("xeyes");
    await wait(2);

    for (let i = 0; i < 100; i++) {
        console.time("activeWindowsByClassName");
        activeWindowsByClassNameSync("xeyes.XEyes");
        console.timeEnd("activeWindowsByClassName");
    }
    await wait(1);

    for (let i = 0; i < 100; i++) {
        try {
            getActiveWindowSync();
        } catch(e) {
            //Voluntary error [Must failed]
            console.error("Voluntary error [Must failed]", e.message);
        }
    }
    const winsXeyes = getWindowsByClassNameSync("xeyes.XEyes");
    console.timeEnd('getWindowsByClassName');
    if (!winsXeyes || !winsXeyes.length)
        throw new Error("Unexpected empty array");
    await wait(1);

    console.time("windowMoveResize");
    windowMoveResizeSync(winsXeyes[0].win_id, 0, 0, 0, 500, 500);
    console.timeEnd("windowMoveResize");
    await wait(1);

    console.time("windowMoveResize");
    windowMoveResizeSync(winsXeyes[0].win_id, 0, 0, 0, 1000, 1000);
    console.timeEnd("windowMoveResize");
    await wait(1);

    console.time("windowState");
    windowStateSync(winsXeyes[0].win_id, "add", "maximized_vert", "maximized_horz");
    console.timeEnd("windowState");
    await wait(1);

    console.time("windowState");
    windowStateSync(winsXeyes[0].win_id, "remove", "maximized_vert", "maximized_horz");
    console.timeEnd("windowState");
    await wait(1);

    console.time("windowState");
    windowStateSync(winsXeyes[0].win_id, "add", "fullscreen");
    console.timeEnd("windowState");
    await wait(1);

    console.time("windowState");
    windowStateSync(winsXeyes[0].win_id, "remove", "fullscreen");
    console.timeEnd("windowState");
    await wait(1);

    console.time("windowMinimize");
    windowMinimize(winsXeyes[0].win_id);
    console.timeEnd("windowMinimize");
    await wait(1);

    console.time("activeWindowById");
    activeWindowByIdSync(winsXeyes[0].win_id);
    console.timeEnd("activeWindowById");

    console.time("windowState");
    windowStateSync(winsXeyes[0].win_id, "add", "fullscreen");
    console.timeEnd("windowState");
    await wait(1);

    console.time("closeWindowsByClassName");
    closeWindowsByClassNameSync("xeyes.XEyes");
    console.timeEnd("closeWindowsByClassName");
    exec("firefox");
    await wait(5);

    console.time("getWindowsByClassName");
    let firefoxWindows = getWindowsByClassNameSync("Navigator.Firefox");
    console.timeEnd("getWindowsByClassName");
    let lastWindow = firefoxWindows[firefoxWindows.length - 1];

    console.time("windowAllowAllSizes");
    windowAllowAllSizesSync(lastWindow.win_id);
    console.timeEnd("windowAllowAllSizes");

    console.time("getWindowsByClassName");
    firefoxWindows = getWindowsByClassNameSync("Navigator.Firefox");
    console.timeEnd("getWindowsByClassName");
    lastWindow = firefoxWindows[firefoxWindows.length - 1];

    if (lastWindow.WM_NORMAL_HINTS) {
        const {
            max_width,
            max_height,
            min_width,
            min_height
        } = lastWindow.WM_NORMAL_HINTS;
        if (!(max_width  !== 0  ||
            max_height   !== 0  ||
            min_width    !== 0  ||
            min_height   !== 0))
                throw new Error("window_allow_all_sizes failed");
    } else {
        throw new Error("WM_NORMAL_HINTS not provided");
    }

    console.time("windowMinimizeSync");
    windowMinimizeSync(lastWindow.win_id);
    console.timeEnd("windowMinimizeSync");
    await wait(3);

    console.time("windowRaiseSync");
    windowRaiseSync(lastWindow.win_id);
    console.timeEnd("windowRaiseSync");
    await wait(5);

    console.time("closeWindowByIdSync");
    closeWindowByIdSync(lastWindow.win_id);
    console.timeEnd("closeWindowByIdSync");

})()
.then(() => {
    console.log("ALL TEST ENDED WITH SUCESS");
})
.catch(e => {
    console.error("Unexpected error during tests", e);
})
.finally(() => {
    console.log("DONE!");
});