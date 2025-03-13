import { resourceManager } from "@kit.LocalizationKit";


export const onSurfaceCreated: (id: BigInt, manager: resourceManager.ResourceManager) => void;
export const onSurfaceChanged: (id: BigInt, w: number, h: number) => void;
export const onSurfaceDestroyed: (id: BigInt) => void;
export const onEulerAngleChange: (yaw: number, pitch: number) => void;
export const onScaleChange: (fovy: number) => void;