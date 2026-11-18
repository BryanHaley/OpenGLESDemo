// Copyright (C) The MooGL Project
//
// The OpenGLESDemo samples on macOS: a GLFW window with an OpenGL ES 3.0
// context in place of the Android activity, and the images the activity
// loaded for each sample read with stb_image.
//
//   OpenGLESDemo --list
//   OpenGLESDemo <sample> [--frames N]
//   OpenGLESDemo --all [--frames N]
//
// With --frames the sample draws that many frames and checks it drew
// something without a GL error, and the program exits non-zero if not.

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include <GLUtils.h>
#include <ImageDef.h>
#include <stb/stb_image.h>

#include <NativeTriangle.h>
#include <NativeTriangle2.h>
#include <NativeTriangle3.h>
#include <NativeTriangleMapBuffers.h>
#include <NativeTriangleVertextArrayObject.h>
#include <NativeTriangleVertextBufferObject.h>
#include <NativeRectangle.h>
#include <NativeCubeSimpleVertexShader.h>
#include <SimpleTexture2D.h>
#include <SimpleTextureCubeMap.h>
#include <MipMap2D.h>
#include <TextureWrap.h>
#include <MultiTexture.h>
#include <ParticleSystem.h>
#include <ParticleSystemTransformFeedBack.h>
#include <Noise3DRender.h>
#include <TerrainRender.h>
#include <MRT.h>
#include <Shadows.h>
#include <Native1Lesson.h>
#include <Native2Lesson.h>
#include <Native3Lesson.h>
#include <Native4Lesson.h>
#include <Native5Lesson.h>
#include <Native6Lesson.h>
#include <TextureMapSample.h>
#include <FBOSample.h>
#include <FBOLegLengthenSample.h>
#include <CoordSystemSample.h>
#include <BasicLightingSample.h>
#include <MultiLightingsSample.h>
#include <Instancing3DSample.h>
#include <StencilTestingSample.h>
#include <BlendingSample.h>
#include <ParticlesSample2.h>
#include <SkyBoxSample.h>
#include <PBOSample.h>
#include <BezierCurveSample.h>
#include <ScratchCardSample.h>
#include <ShockWaveSample.h>
#include <MRTSample.h>
#include <FBOBlitSample.h>
#include <UniformBufferSample.h>
#include <RGB2YUVSample.h>
#include <AirHockeySample.h>
#include <RotateTexture.h>
#include <GreenScreenMatting.h>
#include <GreenScreenMattingMix.h>
#include <StickerSample.h>
#include <TimeWatermarkStickerSample.h>
#include <BigEyesSample.h>
#include <FaceSlenderSample.h>
#include <BigHeadSample.h>
#include <RotaryHeadSample.h>
#include <AvatarSample.h>

struct Image {
    int index;          // -1 for LoadImage, else LoadMultiImageWithIndex
    const char *path;   // under app/src/main
};

struct Sample {
    const char *name;
    std::function<GLBaseSample *()> make;
    std::vector<Image> images;
};

#define ASSET(p) "assets/" p
#define RES(p) "res/" p

static const char *YANG = ASSET("texture/yangchaoyue.png");

static std::vector<Sample> samples()
{
    return {
        { "triangle", [] { return new NativeTriangle(); }, {} },
        { "triangle2", [] { return new NativeTriangle2(); }, {} },
        { "triangle3", [] { return new NativeTriangle3(); }, {} },
        { "triangle_map_buffers", [] { return new NativeTriangleMapBuffers(); }, {} },
        { "triangle_vao", [] { return new NativeTriangleVAO(); }, {} },
        { "triangle_vbo", [] { return new NativeTriangleVBO(); }, {} },
        { "rectangle", [] { return new NativeRectangle(); }, {} },
        { "cube", [] { return new NativeCubeSimpleVertexShader(); }, {} },
        { "texture_2d", [] { return new SimpleTexture2D(); }, {} },
        { "texture_cube_map", [] { return new SimpleTextureCubeMap(); }, {} },
        { "mipmap_2d", [] { return new MipMap2D(); }, {} },
        { "texture_wrap", [] { return new TextureWrap(); }, {} },
        { "multi_texture", [] { return new MultiTexture(); }, {} },
        { "particle_system", [] { return new ParticleSystem(); }, {} },
        { "particle_system_transform_feedback", [] { return new ParticleSystemTransformFeedBack(); }, {} },
        { "noise_3d", [] { return new Noise3DRender(); }, {} },
        { "terrain", [] { return new TerrainRender(); }, {} },
        { "mrt", [] { return new MRT(); }, {} },
        { "shadows", [] { return new Shadows(); }, {} },
        { "lesson1", [] { return new Native1Lesson(); }, {} },
        { "lesson2", [] { return new Native2Lesson(); }, {} },
        { "lesson3", [] { return new Native3Lesson(); }, {} },
        { "lesson4", [] { return new Native4Lesson(); }, {} },
        { "lesson5", [] { return new Native5Lesson(); }, {} },
        { "lesson6", [] { return new Native6Lesson(); }, {} },
        { "texture_map", [] { return new TextureMapSample(); }, { { -1, YANG } } },
        { "fbo", [] { return new FBOSample(); }, { { -1, YANG } } },
        { "fbo_leg", [] { return new FBOLegLengthenSample(); }, { { -1, ASSET("texture/leg.jpg") } } },
        { "coord_system", [] { return new CoordSystemSample(); }, { { -1, YANG } } },
        { "basic_lighting", [] { return new BasicLightingSample(); }, { { -1, YANG } } },
        { "multi_lighting", [] { return new MultiLightingsSample(); }, { { -1, YANG } } },
        { "instancing", [] { return new Instancing3DSample(); }, { { -1, YANG } } },
        { "stencil", [] { return new StencilTestingSample(); }, { { -1, YANG } } },
        { "pbo", [] { return new PBOSample(); }, { { -1, YANG } } },
        { "blending", [] { return new BlendingSample(); },
          { { 0, RES("mipmap-xhdpi/board_texture.png") }, { 1, RES("mipmap-xhdpi/floor.png") },
            { 2, RES("mipmap-xhdpi/window.png") } } },
        { "particles2", [] { return new ParticlesSample2(); }, { { -1, RES("mipmap-xhdpi/board_texture.png") } } },
        { "skybox", [] { return new SkyBoxSample(); },
          { { 0, RES("mipmap-xhdpi/direction_right.jpg") }, { 1, RES("mipmap-xhdpi/direction_left.jpg") },
            { 2, RES("mipmap-xhdpi/direction_top.jpg") }, { 3, RES("mipmap-xhdpi/direction_bottom.jpg") },
            { 4, RES("mipmap-xhdpi/direction_back.jpg") }, { 5, RES("mipmap-xhdpi/direction_front.jpg") } } },
        { "bezier_curve", [] { return new BezierCurveSample(); }, {} },
        { "scratch_card", [] { return new ScratchCardSample(); }, { { -1, RES("mipmap-xhdpi/yifei.jpg") } } },
        { "big_eyes", [] { return new BigEyesSample(); }, { { -1, RES("mipmap-xhdpi/yifei.jpg") } } },
        { "face_slender", [] { return new FaceSlenderSample(); }, { { -1, RES("mipmap-xhdpi/yifei.jpg") } } },
        { "big_head", [] { return new BigHeadSample(); }, { { -1, RES("mipmap-xhdpi/huge.jpg") } } },
        { "rotary_head", [] { return new RotaryHeadSample(); }, { { -1, RES("mipmap-xhdpi/huge.jpg") } } },
        { "avatar", [] { return new AvatarSample(); },
          { { 0, RES("mipmap-xhdpi/avatar_a.jpg") }, { 1, RES("mipmap-xhdpi/avatar_b.png") },
            { 2, RES("mipmap-xhdpi/avatar_c.png") } } },
        { "shock_wave", [] { return new ShockWaveSample(); }, { { -1, RES("mipmap-xhdpi/lye.jpg") } } },
        { "mrt2", [] { return new MRTSample(); }, { { -1, RES("mipmap-xhdpi/lye.jpg") } } },
        { "fbo_blit", [] { return new FBOBlitSample(); }, { { -1, RES("mipmap-xhdpi/lye.jpg") } } },
        { "ubo", [] { return new UniformBufferSample(); }, { { -1, RES("mipmap-xhdpi/lye.jpg") } } },
        { "rgb2yuv", [] { return new RGB2YUVSample(); }, { { -1, RES("mipmap-xhdpi/lye.jpg") } } },
        { "air_hockey", [] { return new AirHockeySample(); }, { { -1, RES("drawable-nodpi/air_hockey_surface.png") } } },
        { "rotate_texture", [] { return new RotateTexture(); }, { { -1, RES("mipmap-xhdpi/luna.png") } } },
        { "green_screen", [] { return new GreenScreenMatting(); }, { { -1, RES("mipmap-xhdpi/green_screen_mm.jpg") } } },
        { "green_screen_mix", [] { return new GreenScreenMattingMix(); },
          { { 0, RES("mipmap-xhdpi/green_screen_mm.jpg") }, { 1, RES("mipmap-xhdpi/green_screen_background.jpg") } } },
        { "sticker", [] { return new StickerSample(); },
          { { -1, YANG }, { 0, RES("mipmap-xhdpi/sticker_c_plus.png") }, { 1, RES("mipmap-xhdpi/sticker_b_plus.png") },
            { 2, RES("mipmap-xhdpi/sticker_a_minute.png") }, { 3, RES("mipmap-xhdpi/sticker_a_plus.png") } } },
        { "time_watermark", [] { return new TimeWatermarkStickerSample(); }, { { -1, RES("mipmap-xhdpi/yifei.jpg") } } },
    };
}

static std::string root;

static bool loadImage(GLBaseSample *s, const Image &img)
{
    int w, h, n;
    unsigned char *px = stbi_load((root + "/" + img.path).c_str(), &w, &h, &n, 4);

    if (px == nullptr)
    {
        fprintf(stderr, "can't read %s\n", img.path);
        return false;
    }

    NativeImage image;
    image.width = w;
    image.height = h;
    image.format = IMAGE_FORMAT_RGBA;
    image.ppPlane[0] = px;

    if (img.index < 0)
        s->LoadImage(&image);
    else
        s->LoadMultiImageWithIndex(img.index, &image);

    stbi_image_free(px);
    return true;
}

// a frame of one colour is a frame nothing drew into
static bool drewSomething(int w, int h)
{
    std::vector<unsigned char> px((size_t)w * h * 4);
    std::set<unsigned> colours;

    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
    for (size_t i = 0; i < px.size() && colours.size() < 2; i += 4)
        colours.insert((unsigned)px[i] | px[i + 1] << 8 | px[i + 2] << 16);

    return colours.size() > 1;
}

// each sample in a window and context of its own, as each was its own
// activity on Android
static bool run(const Sample &sample, int frames)
{
    GLFWwindow *window = glfwCreateWindow(720, 800, sample.name, nullptr, nullptr);

    if (window == nullptr)
    {
        fprintf(stderr, "no OpenGL ES 3.0 window\n");
        return false;
    }

    glfwMakeContextCurrent(window);

    GLBaseSample *s = sample.make();
    int w, h;

    for (const Image &img : sample.images)
        if (!loadImage(s, img))
        {
            printf("  [FAIL] %s (missing image)\n", sample.name);
            return false;
        }

    while (glGetError() != GL_NO_ERROR)
        ;

    glfwGetFramebufferSize(window, &w, &h);
    s->Create();
    s->Change(w, h);

    for (int i = 0; frames < 0 ? !glfwWindowShouldClose(window) : i < frames; i++)
    {
        // a finger dragged across the middle, or the mouse while a button is down
        if (frames > 0)
            s->SetTouchLocation(w * (0.2f + 0.6f * i / frames), h * 0.5f);
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            double x, y;
            int ww, wh;

            glfwGetCursorPos(window, &x, &y);
            glfwGetWindowSize(window, &ww, &wh);
            s->SetTouchLocation((float)(x * w / ww), (float)(y * h / wh));
        }

        s->Draw();
        if (i == frames - 1)
            break;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    bool ok = true;

    if (frames >= 0)
    {
        GLenum e = glGetError();
        bool drew = drewSomething(w, h);

        ok = e == GL_NO_ERROR && drew;
        printf("  [%s] %s%s\n", ok ? "ok  " : "FAIL", sample.name,
               e != GL_NO_ERROR ? " (GL error)" : !drew ? " (drew nothing)" : "");
        glfwSwapBuffers(window);
    }

    s->Shutdown();
    delete s;
    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(window);
    return ok;
}

int main(int argc, char **argv)
{
    std::vector<Sample> all = samples();
    const char *name = nullptr;
    int frames = -1;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--list"))
        {
            for (const Sample &s : all)
                printf("%s\n", s.name);
            return 0;
        }
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc)
            frames = atoi(argv[++i]);
        else
            name = argv[i];
    }

    if (name == nullptr)
    {
        fprintf(stderr, "usage: %s --list | <sample> | --all [--frames N]\n", argv[0]);
        return 2;
    }

    // the app's files, found from the binary's source tree
    root = OPENGLESDEMO_MAIN_DIR;
    GLUtils::setAssetRoot((root + "/assets").c_str());

    if (!glfwInit())
        return 77;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    int failed = 0;

    for (const Sample &s : all)
        if (!strcmp(name, "--all") || !strcmp(name, s.name))
            failed += !run(s, frames);

    glfwTerminate();

    if (frames >= 0)
        printf("%s\n", failed ? "FAILED" : "passed");
    return failed ? 1 : 0;
}
