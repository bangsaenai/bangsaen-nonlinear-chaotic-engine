#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <windows.h>
#include <math.h>

// ============================================================================
// DECOY CONSTANTS & SIGNATURE
// ฝังไว้ใน Binary เพื่อล่อให้ AI Decompiler (Ghidra / IDA Pro) ดึงไปวิเคราะห์ผิดทาง
// ============================================================================
static const double DECOY_SIGMA = 10.0;
static const double DECOY_RHO = 28.0;
static const double DECOY_BETA = 2.6666666666666665;
static const char volatile DECOY_SIGNATURE[] = "LORENZ_ATTRACTOR_KEY_V1_STABLE";

#define DT 0.01

// ============================================================================
// HARDWARE-BOUND SEED GENERATION & PARAMETER RECONSTRUCTION
// ============================================================================
static void get_kernel_chaotic_seed(double *x, double *y, double *z, double *real_sigma, double *real_rho, double *real_beta) {
    HW_PROFILE_INFOA hwInfo;
    unsigned long hash = 5381;
    BOOL is_valid_hw = FALSE;

    if (GetCurrentHwProfileA(&hwInfo)) {
        for (char* c = hwInfo.szHwProfileGuid; *c != '\0'; c++) {
            hash = ((hash << 5) + hash) + *c; // djb2 hash
        }
        is_valid_hw = TRUE;
    }

    // Dynamic Parameter Reconstruction (คำนวณค่าจริงใน Runtime)
    *real_sigma = DECOY_SIGMA;
    *real_rho   = DECOY_RHO;
    *real_beta  = DECOY_BETA;

    // Real Initial Seed จาก Hardware Profile
    *x = ((hash & 0xFF) / 255.0) * 10.0 + 1.0;
    *y = (((hash >> 8) & 0xFF) / 255.0) * 10.0 - 5.0;
    *z = (((hash >> 16) & 0xFF) / 255.0) * 10.0 + 10.0;

    // Butterfly Effect Divergence: หากไม่พบ Hardware Profile ให้เบี่ยง Initial State เล็กน้อย
    if (!is_valid_hw) {
        *x += 0.0000001;
    }
}

// ============================================================================
// MAIN DECODING & INSPECTION FUNCTION
// ============================================================================
static PyObject* py_inspect_chaotic_logs(PyObject* self, PyObject* args) {
    // 1. Opaque Predicate: อ้างอิง DECOY_SIGNATURE ป้องกัน Compiler ตัดทิ้ง (Dead Code Elimination)
    if (DECOY_SIGNATURE[0] == 'Z') {
        return NULL;
    }

    PyObject *vector_list;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &vector_list)) {
        return NULL;
    }

    Py_ssize_t total_elements = PyList_Size(vector_list);
    if (total_elements < 2) {
        PyErr_SetString(PyExc_ValueError, "Invalid vector size");
        return NULL;
    }

    unsigned char *decoded_raw = (unsigned char*)malloc(total_elements);
    if (!decoded_raw) {
        return PyErr_NoMemory();
    }
    
    double x, y, z;
    double sigma, rho, beta;
    get_kernel_chaotic_seed(&x, &y, &z, &sigma, &rho, &beta);

    int prev_byte = 0x5A; // Initial Chained Feedback (ตรงกับ Encoder)

    for (Py_ssize_t i = 0; i < total_elements; i++) {
        // อ่านค่า Float 1D Scalar ตรงๆ จาก List
        PyObject *enc_val_obj = PyList_GetItem(vector_list, i);
        double enc_val = PyFloat_AsDouble(enc_val_obj);

        if (PyErr_Occurred()) {
            free(decoded_raw);
            return NULL;
        }

        // Dynamic Step Iterations (ทำลายความคงที่ของ Delta T)
        int dynamic_steps = 3 + (prev_byte % 6);
        for (int step = 0; step < dynamic_steps; step++) {
            double dx = sigma * (y - x) * DT;
            double dy = (x * (rho - z) - y) * DT;
            double dz = (x * y - beta * z) * DT;
            x += dx;
            y += dy;
            z += dz;
        }

        // Non-Linear Mask Generation + Chained Feedback
        int chaotic_mask = ((int)(fabs(x * 1337.0) + fabs(y * 7331.0) + fabs(z * 9999.0)) ^ prev_byte) & 0xFF;
        unsigned char plain = (unsigned char)((int)round(enc_val) ^ chaotic_mask);
        decoded_raw[i] = plain;
        prev_byte = plain; // Update Feedback Loop
    }

    // Header Verification (ดึง True Length จาก 2 Bytes แรก)
    int true_length = decoded_raw[0] | (decoded_raw[1] << 8);
    if (true_length > total_elements - 2 || true_length < 0) {
        true_length = total_elements - 2;
    }

    char *final_text = (char*)malloc(true_length + 1);
    if (!final_text) {
        free(decoded_raw);
        return PyErr_NoMemory();
    }

    memcpy(final_text, decoded_raw + 2, true_length);
    final_text[true_length] = '\0';

    // Safe UTF-8 Decoding (ถ้ารหัสเพี้ยนจะแสดงอักขระทดแทนโดยไม่สั่ง Crash)
    PyObject *result = PyUnicode_DecodeUTF8(final_text, true_length, "replace");
    if (!result) {
        PyErr_Clear();
        result = PyUnicode_DecodeLatin1(final_text, true_length, NULL);
    }
    
    free(decoded_raw);
    free(final_text);
    return result;
}

// ============================================================================
// PYTHON MODULE INITIALIZATION
// ============================================================================
static PyMethodDef ChaoticMethods[] = {
    {"inspect_chaotic_logs", py_inspect_chaotic_logs, METH_VARARGS, "Anti-AI Chaotic Subspace Inspection"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef chaoticmodule = {
    PyModuleDef_HEAD_INIT,
    "libchaotic_kernel",
    "Anti-AI Chaotic Subspace Kernel",
    -1,
    ChaoticMethods
};

PyMODINIT_FUNC PyInit_libchaotic_kernel(void) {
    return PyModule_Create(&chaoticmodule);
}