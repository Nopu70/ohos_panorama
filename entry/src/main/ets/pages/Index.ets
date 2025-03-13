import rendererApi from 'libentry.so';
import { resourceManager } from '@kit.LocalizationKit';
import { fileIo, picker } from '@kit.CoreFileKit'
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import hilog from '@ohos.hilog';
import { buffer } from '@kit.ArkTS';

class MXComponentController extends XComponentController {
  manager: resourceManager.ResourceManager;
  constructor(manager: resourceManager.ResourceManager) {
    super();
    this.manager = manager;
  }
  onSurfaceCreated(surfaceId: string): void {
    rendererApi.onSurfaceCreated(BigInt(surfaceId), this.manager);
  }
  onSurfaceChanged(surfaceId: string, rect: SurfaceRect): void {
    rendererApi.onSurfaceChanged(BigInt(surfaceId), rect.surfaceWidth, rect.surfaceHeight);
  }
  onSurfaceDestroyed(surfaceId: string): void {
    rendererApi.onSurfaceDestroyed(BigInt(surfaceId));
  }
}

@Entry
@Component
struct Index {

  ctrl: ImageAnalyzerController = new ImageAnalyzerController()

  //视锥体夹角（0-45°）
  fovy: number = Math.PI / 4;

  //偏航角
  yaw: number = 0.0;
  //俯仰角
  pitch: number = 0.0;

  openPhoto() {
    let selectOption = new photoAccessHelper.PhotoSelectOptions();
    selectOption.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE;
    selectOption.maxSelectNumber = 5;
    let photoPicker = new photoAccessHelper.PhotoViewPicker();
    photoPicker.select(selectOption).then((result) => {
      hilog.debug(0xf0, "Index", JSON.stringify(result.photoUris))
    }).catch((err: BusinessError) => {
      hilog.debug(0xf0, "Index", err.message)
    })
  }

  async openFile() {
    let selectOption = new picker.DocumentSelectOptions();
    selectOption.fileSuffixFilters = ['图片(.png, .jpg)|.png,.jpg', '文档|.txt', '视频|.mp4', '.pdf'];
    selectOption.maxSelectNumber = 5;
    let photoPicker = new picker.DocumentViewPicker();
    photoPicker.select(selectOption).then((result) => {
      hilog.debug(0xf0, "Index", JSON.stringify(result))
      result.forEach((uri) => {
        let f = fileIo.openSync(uri, fileIo.OpenMode.READ_ONLY);
        let buf = new ArrayBuffer(1024);
        fileIo.readSync(f.fd, buf)

        hilog.debug(0xf0, "Index", buffer.from(buf).toString())
        fileIo.close(f)
      })
    }).catch((err: BusinessError) => {
      hilog.debug(0xf0, "Index", err.message)
    })
  }

  build() {
    Stack({
      alignContent: Alignment.BottomEnd
    }) {
      XComponent({
        id: "surface",
        type: XComponentType.SURFACE,
        controller: new MXComponentController(getContext().resourceManager)
      }).gesture(GestureGroup(GestureMode.Exclusive, PanGesture()
        .onActionUpdate((event) => {
          if (event) {
            let step = this.fovy / 600;
            let yawOffset = this.yaw - (event.offsetX * step);
            let pitchOffset = this.pitch + (event.offsetY * step);
            pitchOffset = Math.max(Math.min(pitchOffset, 1.5), -1.5);

            rendererApi.onEulerAngleChange(yawOffset, pitchOffset);
          }
        }).onActionEnd((event) => {
          if (event) {
            let step = this.fovy / 600;
            this.yaw -= (event.offsetX * step);
            this.pitch += (event.offsetY * step);
            this.pitch = Math.max(Math.min(this.pitch, 1.5), -1.5);

            rendererApi.onEulerAngleChange(this.yaw, this.pitch);
          }
        }), PinchGesture().onActionUpdate((event) => {
          let fovy = this.fovy / event.scale;
          fovy = Math.min(Math.max(0.1, fovy), Math.PI / 4);
          console.log(`on__scale__ ${event.scale}====${fovy}`)
          rendererApi.onScaleChange(fovy);
        }).onActionEnd((event) => {
          this.fovy = this.fovy / event.scale;
          this.fovy = Math.min(Math.max(0.1, this.fovy), Math.PI / 4);
          console.log(`on__scale__ ${event.scale}====${this.fovy}`)
          rendererApi.onScaleChange(this.fovy);
        })))

      Button("打开")
        .margin({
          right: 20,
          bottom: 20
        }).onClick(() => {
          this.openFile();
        })
    }
  }
}
