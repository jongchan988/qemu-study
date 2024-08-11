#include "qemu/osdep.h"
#include "qemu/datadir.h"
#include "hw/loader.h"
#include "boot.h"
#include "qemu/error-report.h"

bool avr32_load_firmware(AVR32ACPU *cpu, MachineState *ms, MemoryRegion *program_mr, const char *firmware){
    g_autofree char *filename = NULL;
    int bytes_loaded;

    // QEMU가 시작될 때 'bios'로 지정된 파일명을 가져옴
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, firmware);
    if (filename == NULL) {
        error_report("Cannot find firmware image '%s'", firmware);
        return false;
    }
    // 내장 함수를 사용하여 파일에서 에뮬레이트된 메모리로 펌웨어를 로드
    bytes_loaded = load_image_mr(filename, program_mr);

    if (bytes_loaded < 0) {
        error_report("Unable to load firmware image %s as ELF or raw binary", firmware);
        return false;
    }
    return true;
}
