#include "Presets.h"

// ============================================================================
//  Factory bank.  Each preset starts from the Init patch and overrides values.
//  Values are plain units (Hz, seconds, choice index, 0/1 for switches).
//
//  Quick index reference
//   Wavetables: 0 Analog Classic (0=sine .25=tri .5=saw .75=square 1=pulse) 1 PWM 2 Warm Saw 3 Formant
//               4 Drawbar Organ 5 Hollow Saw 6 Reso Sweep 7 FM Tines 8 Wavefold 9 Sync Sweep
//               10 Digital Grit 11 Additive Build
//   Warp: 0 Off 1 Sync 2 Bend 3 Mirror 4 FM
//   Filter: 0 Ladder24 1 Ladder12 2 LP12 3 LP24 4 HP12 5 HP24 6 BP 7 Notch
//   Mod src: 1 Env2 2 Env3 3 LFO1 4 LFO2 5 Vel 6 MW 7 AT 8 Key 9 Rnd 10-13 Macro1-4 14 AmpEnv
//   Mod dst: 1 PitchA 2 PitchB 3 PitchAll 4 PosA 5 PosB 6 LvlA 7 LvlB 8 DetA 9 DetB 10 WarpA 11 WarpB
//            12 Cutoff 13 Res 14 Drive 15 Sub 16 Noise 17 Amp 18 Pan 19 LFO1Rate 20 LFO2Rate
//   Divisions: 0 1/32 2 1/16 3 1/16D 4 1/8T 5 1/8 6 1/8D 7 1/4T 8 1/4 9 1/4D 10 1/2 11 1bar 12 2bar 13 4bar
//   Chord: 1 Min 2 Maj 3 Min7 4 Maj7 5 Min9 6 Sus4 7 Fifth 8 Oct 9 DubMin7 10 Stab4ths
//   Mode: 0 Poly 1 Mono 2 Legato     Dist: 0 Soft 1 Hard 2 Tube 3 Fold 4 Crush
// ============================================================================

namespace
{
    using V = std::vector<std::pair<juce::String, float>>;

    // Common building blocks --------------------------------------------------
    V monoBass (float glide = 0.03f) { return { { "mode", 1 }, { "glide", glide }, { "vel", 0.3f }, { "e1_a", 0.001f }, { "e1_r", 0.07f } }; }
    V legatoBass (float glide = 0.05f) { return { { "mode", 2 }, { "glide", glide }, { "vel", 0.3f }, { "e1_a", 0.001f }, { "e1_r", 0.07f } }; }
    V subOn (float lvl = 0.55f) { return { { "sub_on", 1 }, { "sub_wave", 0 }, { "sub_oct", 0 }, { "sub_level", lvl }, { "sub_direct", 1 } }; }
    V punch (int slot, float amt, float decay)
    {
        const juce::String s = "m" + juce::String (slot);
        return { { s + "_src", 2 }, { s + "_dst", 3 }, { s + "_amt", amt }, { "e3_a", 0.0005f }, { "e3_d", decay }, { "e3_s", 0.0f } };
    }
    V mod (int slot, int src, int dst, float amt)
    {
        const juce::String s = "m" + juce::String (slot);
        return { { s + "_src", (float) src }, { s + "_dst", (float) dst }, { s + "_amt", amt } };
    }
    V delay (int div, float fb, float mix, float tone = 0.55f, bool pp = true)
    {
        return { { "dly_on", 1 }, { "dly_sync", 1 }, { "dly_div", (float) div }, { "dly_fb", fb }, { "dly_mix", mix },
                 { "dly_tone", tone }, { "dly_pp", pp ? 1.0f : 0.0f } };
    }
    V reverb (float size, float mix, float damp = 0.5f) { return { { "rev_on", 1 }, { "rev_size", size }, { "rev_mix", mix }, { "rev_damp", damp } }; }
    V chorus (float mix = 0.35f, float depth = 0.35f, float rate = 0.6f) { return { { "cho_on", 1 }, { "cho_mix", mix }, { "cho_depth", depth }, { "cho_rate", rate } }; }
    V dist (int type, float drive, float mix, float tone = 0.7f) { return { { "dist_on", 1 }, { "dist_type", (float) type }, { "dist_drive", drive }, { "dist_mix", mix }, { "dist_tone", tone } }; }
    V vibrato (int slot, float amt = 0.006f, float rise = 0.45f)
    {
        auto v = mod (slot, 4, 3, amt);
        v.insert (v.end(), { { "l2_shape", 0 }, { "l2_sync", 0 }, { "l2_rate", 5.5f }, { "l2_mode", 1 }, { "l2_delay", rise } });
        return v;
    }
    V comp (float thresh = -18.0f, float ratio = 4.0f, float makeup = 3.0f)
    {
        return { { "comp_on", 1 }, { "comp_thresh", thresh }, { "comp_ratio", ratio }, { "comp_gain", makeup } };
    }

    V join (std::initializer_list<V> parts)
    {
        V out;
        for (auto& p : parts)
            for (auto& kv : p)
            {
                // later values win (allows overriding building blocks)
                auto it = std::find_if (out.begin(), out.end(), [&] (auto& e) { return e.first == kv.first; });
                if (it != out.end()) it->second = kv.second; else out.push_back (kv);
            }
        return out;
    }

    std::vector<FactoryPreset> build()
    {
        std::vector<FactoryPreset> b;
        auto add = [&b] (const char* folder, const char* name, V values) { b.push_back ({ folder, name, std::move (values) }); };

        add ("Init", "Init", {});

        // =====================================================================
        //  MOODCHILD STYLE - groovy, rolling, warm deep-tech
        // =====================================================================
        const char* F = "Moodchild Style";
        add (F, "Rolling Sub Groove", join ({ monoBass (0.03f), subOn (0.7f), punch (1, 0.12f, 0.03f), mod (2, 10, 12, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.62f }, { "a_rand", 0 }, { "f_type", 0 }, { "f_cutoff", 180 }, { "f_res", 0.25f },
            { "f_drive", 0.3f }, { "f_env", 0.55f }, { "e2_a", 0.001f }, { "e2_d", 0.18f }, { "e2_s", 0.1f },
            { "e1_d", 0.4f }, { "e1_s", 0.85f }, { "analog", 0.4f } } }));

        add (F, "Warm Rubber Bass", join ({ legatoBass (0.06f), punch (1, 0.15f, 0.025f), dist (2, 0.25f, 0.5f), {
            { "a_wt", 2 }, { "a_pos", 0.35f }, { "a_rand", 0 }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.75f }, { "b_oct", -1 },
            { "b_level", 0.5f }, { "b_rand", 0 }, { "f_cutoff", 220 }, { "f_res", 0.45f }, { "f_drive", 0.35f }, { "f_env", 0.6f },
            { "e2_d", 0.22f }, { "e2_s", 0.05f }, { "e1_s", 0.9f } } }));

        add (F, "Mood Pluck", join ({ mod (1, 1, 4, 0.5f), delay (6, 0.3f, 0.18f), reverb (0.55f, 0.18f), {
            { "a_wt", 6 }, { "a_pos", 0.3f }, { "a_uni", 3 }, { "a_det", 0.15f }, { "f_type", 3 }, { "f_cutoff", 600 },
            { "f_res", 0.3f }, { "f_env", 0.65f }, { "e2_d", 0.25f }, { "e2_s", 0 }, { "e2_r", 0.25f },
            { "e1_d", 0.45f }, { "e1_s", 0 }, { "e1_r", 0.35f } } }));

        add (F, "Late Night Keys", join ({ mod (1, 5, 4, 0.4f), chorus (0.35f), reverb (0.5f, 0.2f), {
            { "a_wt", 7 }, { "a_pos", 0.35f }, { "e1_a", 0.002f }, { "e1_d", 1.8f }, { "e1_s", 0.25f }, { "e1_r", 0.6f },
            { "f_type", 2 }, { "f_cutoff", 4500 }, { "f_res", 0.1f }, { "f_key", 0.3f }, { "vel", 0.7f } } }));

        add (F, "Hypno Bleep", join ({ mod (1, 2, 1, 0.5f), delay (5, 0.45f, 0.25f), reverb (0.5f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.1f }, { "a_rand", 0 }, { "e3_a", 0.0005f }, { "e3_d", 0.015f }, { "e3_s", 0 },
            { "e1_a", 0.001f }, { "e1_d", 0.12f }, { "e1_s", 0 }, { "e1_r", 0.1f }, { "f_on", 0 } } }));

        add (F, "Velvet Pad", join ({ mod (1, 3, 12, 0.12f), mod (2, 4, 4, 0.2f), chorus (0.4f), reverb (0.8f, 0.35f), {
            { "a_wt", 2 }, { "a_pos", 0.5f }, { "a_uni", 6 }, { "a_det", 0.35f }, { "a_blend", 0.7f },
            { "b_on", 1 }, { "b_wt", 11 }, { "b_pos", 0.6f }, { "b_uni", 4 }, { "b_det", 0.25f }, { "b_oct", 1 }, { "b_level", 0.35f },
            { "f_cutoff", 1400 }, { "f_res", 0.15f }, { "e1_a", 1.2f }, { "e1_d", 1.0f }, { "e1_s", 0.85f }, { "e1_r", 2.2f },
            { "l1_sync", 0 }, { "l1_rate", 0.15f }, { "l1_mode", 0 }, { "l2_sync", 0 }, { "l2_rate", 0.08f }, { "l2_shape", 6 },
            { "l2_mode", 0 }, { "analog", 0.5f } } }));

        add (F, "Swing Stab", join ({ delay (6, 0.4f, 0.25f, 0.45f), reverb (0.6f, 0.2f), {
            { "chord", 9 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 2 }, { "a_det", 0.1f },
            { "f_cutoff", 900 }, { "f_res", 0.35f }, { "f_env", 0.5f }, { "e2_d", 0.2f }, { "e2_s", 0.1f },
            { "e1_d", 0.35f }, { "e1_s", 0 }, { "e1_r", 0.3f } } }));

        add (F, "Bouncy Mid Bass", join ({ monoBass (0.02f), subOn (0.55f), mod (1, 3, 4, 0.25f), {
            { "a_wt", 1 }, { "a_pos", 0.3f }, { "a_rand", 0 }, { "l1_div", 2 }, { "l1_shape", 1 },
            { "f_cutoff", 350 }, { "f_res", 0.5f }, { "f_drive", 0.4f }, { "f_env", 0.45f }, { "e2_d", 0.15f }, { "e2_s", 0.2f },
            { "e1_d", 0.3f }, { "e1_s", 0.7f } } }));

        add (F, "Wonky Lead", join ({ mod (1, 3, 4, 0.35f), vibrato (2, 0.006f, 0.4f), delay (7, 0.35f, 0.2f), dist (0, 0.2f, 0.6f), {
            { "a_wt", 9 }, { "a_pos", 0.3f }, { "l1_shape", 6 }, { "l1_div", 5 }, { "mode", 2 }, { "glide", 0.08f },
            { "f_type", 2 }, { "f_cutoff", 3500 }, { "f_res", 0.2f }, { "e1_s", 0.8f }, { "e1_r", 0.25f } } }));

        add (F, "Afterglow Chords", join ({ chorus (0.35f), delay (6, 0.35f, 0.22f), reverb (0.7f, 0.28f), {
            { "chord", 5 }, { "voices", 16 }, { "a_wt", 2 }, { "a_pos", 0.55f }, { "a_uni", 3 }, { "a_det", 0.18f },
            { "b_on", 1 }, { "b_wt", 3 }, { "b_pos", 0.4f }, { "b_level", 0.3f }, { "f_type", 3 }, { "f_cutoff", 1100 },
            { "f_res", 0.2f }, { "f_env", 0.35f }, { "e2_a", 0.01f }, { "e2_d", 0.6f }, { "e2_s", 0.3f },
            { "e1_a", 0.005f }, { "e1_d", 1.2f }, { "e1_s", 0.3f }, { "e1_r", 0.8f } } }));

        // =====================================================================
        //  DEEPERFECT STYLE - driving, dark, hypnotic
        // =====================================================================
        F = "Deeperfect Style";
        add (F, "Driving Reese", join ({ legatoBass (0.05f), subOn (0.6f), mod (1, 3, 12, 0.15f), dist (2, 0.3f, 0.6f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 4 }, { "a_det", 0.35f }, { "a_blend", 0.8f }, { "a_width", 0.5f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_uni", 4 }, { "b_det", 0.45f }, { "b_level", 0.6f }, { "b_width", 0.5f },
            { "f_cutoff", 420 }, { "f_res", 0.3f }, { "f_drive", 0.5f }, { "l1_div", 11 }, { "l1_mode", 0 },
            { "e1_a", 0.005f }, { "e1_s", 1.0f }, { "e1_r", 0.1f }, { "analog", 0.5f } } }));

        add (F, "Rumble Low End", join ({ monoBass (0.0f), dist (0, 0.45f, 1.0f, 0.3f), reverb (0.7f, 0.25f, 0.9f), {
            { "a_wt", 0 }, { "a_pos", 0.15f }, { "a_rand", 0 }, { "sub_on", 1 }, { "sub_oct", 0 }, { "sub_level", 0.6f },
            { "f_cutoff", 120 }, { "f_res", 0.35f }, { "f_drive", 0.8f }, { "e1_a", 0.002f }, { "e1_d", 0.6f }, { "e1_s", 0.4f },
            { "e1_r", 0.4f } } }));

        add (F, "Acid Groove 303", join ({ legatoBass (0.07f), mod (1, 5, 12, 0.25f), mod (2, 10, 12, 0.5f), dist (2, 0.35f, 0.6f),
                                            delay (3, 0.35f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_rand", 0 }, { "f_cutoff", 260 }, { "f_res", 0.78f }, { "f_drive", 0.45f },
            { "f_env", 0.6f }, { "e2_a", 0.001f }, { "e2_d", 0.22f }, { "e2_s", 0 }, { "e2_r", 0.1f },
            { "e1_d", 0.3f }, { "e1_s", 0.8f }, { "e1_r", 0.05f }, { "vel", 0.4f } } }));

        add (F, "Hypnotic Sequence", join ({ mod (1, 3, 4, 0.3f), delay (6, 0.55f, 0.25f), reverb (0.6f, 0.2f), {
            { "a_wt", 6 }, { "a_pos", 0.45f }, { "l1_div", 13 }, { "l1_shape", 1 }, { "l1_mode", 0 },
            { "f_type", 3 }, { "f_cutoff", 1400 }, { "f_res", 0.35f }, { "f_env", 0.3f }, { "e2_d", 0.2f },
            { "e1_d", 0.25f }, { "e1_s", 0 }, { "e1_r", 0.2f } } }));

        add (F, "Dark Stab", join ({ reverb (0.75f, 0.3f), delay (6, 0.3f, 0.18f), {
            { "chord", 1 }, { "voices", 16 }, { "a_wt", 5 }, { "a_pos", 0.6f }, { "a_uni", 2 }, { "a_det", 0.2f },
            { "f_cutoff", 700 }, { "f_res", 0.4f }, { "f_env", 0.45f }, { "f_drive", 0.3f }, { "e2_d", 0.18f },
            { "e1_d", 0.3f }, { "e1_s", 0 }, { "e1_r", 0.25f } } }));

        add (F, "Warehouse Lead", join ({ mod (1, 1, 4, 0.35f), dist (0, 0.3f, 0.6f), delay (6, 0.35f, 0.2f), reverb (0.6f, 0.15f), {
            { "a_wt", 8 }, { "a_pos", 0.35f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_oct", 1 }, { "b_level", 0.25f },
            { "f_type", 2 }, { "f_cutoff", 2500 }, { "f_res", 0.25f }, { "f_env", 0.3f }, { "mode", 2 }, { "glide", 0.04f },
            { "e1_s", 0.8f }, { "e1_r", 0.2f } } }));

        add (F, "Tension Riser", join ({ mod (1, 1, 1, 0.5f), mod (2, 1, 4, 1.0f), reverb (0.9f, 0.4f), delay (8, 0.4f, 0.2f), {
            { "a_wt", 9 }, { "a_pos", 0.0f }, { "a_level", 0.5f }, { "a_uni", 4 }, { "a_det", 0.3f },
            { "noise_on", 1 }, { "noise_type", 0 }, { "noise_level", 0.7f }, { "f_type", 6 }, { "f_cutoff", 200 },
            { "f_res", 0.5f }, { "f_env", 0.9f }, { "e2_a", 8.0f }, { "e2_s", 1.0f }, { "e2_r", 2.0f },
            { "e1_a", 6.0f }, { "e1_s", 1.0f }, { "e1_r", 2.0f }, { "mode", 1 } } }));

        add (F, "Deep Pulse Bass", join ({ monoBass (0.02f), subOn (0.6f), mod (1, 3, 4, 0.3f), {
            { "a_wt", 1 }, { "a_pos", 0.2f }, { "a_rand", 0 }, { "l1_sync", 0 }, { "l1_rate", 0.3f }, { "l1_mode", 0 },
            { "f_cutoff", 300 }, { "f_res", 0.35f }, { "f_env", 0.4f }, { "e2_d", 0.3f }, { "e2_s", 0.15f }, { "e1_s", 0.85f } } }));

        add (F, "Metal Perc Synth", join ({ punch (1, 0.3f, 0.01f), reverb (0.4f, 0.2f), delay (3, 0.3f, 0.15f), {
            { "a_wt", 10 }, { "a_pos", 0.5f }, { "a_warp", 4 }, { "a_warpamt", 0.6f },
            { "b_on", 1 }, { "b_wt", 7 }, { "b_pos", 0.7f }, { "b_oct", 1 }, { "b_semi", 7 }, { "b_level", 0.0f },
            { "e1_a", 0.001f }, { "e1_d", 0.09f }, { "e1_s", 0 }, { "e1_r", 0.08f },
            { "f_type", 4 }, { "f_cutoff", 350 }, { "f_res", 0.2f } } }));

        add (F, "Tunnel Pad", join ({ mod (1, 3, 4, 0.4f), reverb (0.9f, 0.4f), delay (9, 0.35f, 0.2f), {
            { "a_wt", 3 }, { "a_pos", 0.3f }, { "a_uni", 5 }, { "a_det", 0.3f }, { "l1_sync", 0 }, { "l1_rate", 0.07f },
            { "l1_mode", 1 }, { "f_type", 3 }, { "f_cutoff", 1600 }, { "f_res", 0.3f },
            { "e1_a", 2.0f }, { "e1_d", 1.0f }, { "e1_s", 0.9f }, { "e1_r", 3.0f },
            { "pha_on", 1 }, { "pha_rate", 0.1f }, { "pha_mix", 0.4f } } }));

        // =====================================================================
        //  TOMANGO STYLE - funky, vocal, playful
        // =====================================================================
        F = "Tomango Style";
        add (F, "Funk Slap Bass", join ({ monoBass (0.015f), subOn (0.45f), mod (1, 1, 4, 0.45f), mod (2, 5, 4, 0.2f), comp (-20.0f, 4.0f, 3.0f), {
            { "a_wt", 8 }, { "a_pos", 0.3f }, { "a_rand", 0 }, { "e2_a", 0.001f }, { "e2_d", 0.12f }, { "e2_s", 0 },
            { "f_cutoff", 600 }, { "f_res", 0.35f }, { "f_env", 0.45f }, { "e1_d", 0.35f }, { "e1_s", 0.5f }, { "vel", 0.6f } } }));

        add (F, "Talkbox Lead", join ({ mod (1, 3, 4, 0.6f), dist (2, 0.25f, 0.5f), chorus (0.25f), delay (6, 0.3f, 0.18f), {
            { "a_wt", 3 }, { "a_pos", 0.2f }, { "l1_div", 5 }, { "l1_shape", 1 },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_level", 0.3f }, { "b_uni", 2 }, { "b_det", 0.1f },
            { "f_type", 2 }, { "f_cutoff", 3000 }, { "f_res", 0.25f }, { "mode", 2 }, { "glide", 0.06f },
            { "e1_s", 0.85f }, { "e1_r", 0.2f } } }));

        add (F, "Organ Hit", join ({ chorus (0.4f, 0.4f, 0.8f), reverb (0.5f, 0.2f), {
            { "a_wt", 4 }, { "a_pos", 0.55f }, { "e1_a", 0.001f }, { "e1_d", 0.5f }, { "e1_s", 0.4f }, { "e1_r", 0.2f },
            { "f_type", 2 }, { "f_cutoff", 6000 }, { "f_res", 0.1f } } }));

        add (F, "Bubble Bass", join ({ legatoBass (0.05f), subOn (0.4f), mod (2, 1, 10, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.05f }, { "a_rand", 0 }, { "a_warp", 2 }, { "a_warpamt", 0.4f },
            { "m1_src", 2 }, { "m1_dst", 1 }, { "m1_amt", 0.2f }, { "e3_a", 0.0005f }, { "e3_d", 0.06f }, { "e3_s", 0 },
            { "e2_d", 0.15f }, { "e2_s", 0 }, { "f_on", 0 }, { "e1_d", 0.4f }, { "e1_s", 0.6f }, { "e1_r", 0.1f } } }));

        add (F, "Groove Pluck", join ({ delay (6, 0.35f, 0.2f), reverb (0.5f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 3 }, { "a_det", 0.2f },
            { "f_cutoff", 500 }, { "f_res", 0.45f }, { "f_env", 0.6f }, { "e2_d", 0.18f }, { "e2_s", 0 },
            { "e1_d", 0.3f }, { "e1_s", 0 }, { "e1_r", 0.25f } } }));

        add (F, "Disco Rubber", join ({ monoBass (0.04f), dist (0, 0.2f, 0.5f), {
            { "a_wt", 2 }, { "a_pos", 0.7f }, { "a_rand", 0 }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.75f }, { "b_oct", -1 },
            { "b_level", 0.45f }, { "b_rand", 0 }, { "f_cutoff", 280 }, { "f_res", 0.55f }, { "f_env", 0.5f }, { "f_drive", 0.3f },
            { "e2_d", 0.2f }, { "e2_s", 0.1f }, { "e1_s", 0.8f } } }));

        add (F, "Vowel Chop", join ({ mod (1, 3, 4, 1.0f), delay (3, 0.3f, 0.2f), reverb (0.5f, 0.2f), {
            { "a_wt", 3 }, { "a_pos", 0.0f }, { "l1_shape", 5 }, { "l1_div", 2 },
            { "e1_a", 0.001f }, { "e1_d", 0.15f }, { "e1_s", 0 }, { "e1_r", 0.1f }, { "f_type", 2 }, { "f_cutoff", 4500 } } }));

        add (F, "Bright Brass Stab", join ({ reverb (0.6f, 0.2f), {
            { "chord", 2 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 4 }, { "a_det", 0.22f },
            { "b_on", 1 }, { "b_wt", 1 }, { "b_pos", 0.3f }, { "b_uni", 2 }, { "b_det", 0.15f }, { "b_level", 0.5f },
            { "f_cutoff", 900 }, { "f_res", 0.2f }, { "f_env", 0.5f }, { "e2_a", 0.02f }, { "e2_d", 0.35f }, { "e2_s", 0.4f },
            { "e1_a", 0.01f }, { "e1_d", 0.4f }, { "e1_s", 0.6f }, { "e1_r", 0.25f } } }));

        add (F, "Sunset Keys", join ({ mod (1, 5, 4, 0.5f), reverb (0.6f, 0.25f), {
            { "a_wt", 7 }, { "a_pos", 0.25f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.0f }, { "b_oct", 1 }, { "b_level", 0.2f },
            { "e1_a", 0.002f }, { "e1_d", 2.0f }, { "e1_s", 0.3f }, { "e1_r", 0.7f }, { "vel", 0.7f },
            { "pha_on", 1 }, { "pha_mix", 0.3f }, { "pha_rate", 0.2f }, { "f_type", 2 }, { "f_cutoff", 6000 } } }));

        add (F, "Party Lead", join ({ vibrato (1, 0.006f, 0.5f), delay (6, 0.35f, 0.22f), reverb (0.65f, 0.25f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 7 }, { "a_det", 0.4f }, { "a_blend", 0.8f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.75f }, { "b_uni", 3 }, { "b_oct", 1 }, { "b_det", 0.25f }, { "b_level", 0.4f },
            { "f_type", 3 }, { "f_cutoff", 5000 }, { "f_res", 0.15f }, { "e1_a", 0.005f }, { "e1_d", 0.5f }, { "e1_s", 0.8f },
            { "e1_r", 0.4f }, { "mode", 2 }, { "glide", 0.03f } } }));

        // =====================================================================
        //  TOOLROOM STYLE - big tech house, organs, rubber, rave
        // =====================================================================
        F = "Toolroom Style";
        add (F, "Rubber Bass Classic", join ({ monoBass (0.03f), subOn (0.5f), punch (1, 0.1f, 0.02f), dist (2, 0.2f, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_rand", 0 }, { "f_cutoff", 160 }, { "f_res", 0.62f }, { "f_drive", 0.45f },
            { "f_env", 0.55f }, { "e2_a", 0.001f }, { "e2_d", 0.16f }, { "e2_s", 0.05f }, { "e2_r", 0.08f },
            { "e1_d", 0.3f }, { "e1_s", 0.85f } } }));

        add (F, "Classic House Organ", join ({ chorus (0.3f, 0.35f, 0.6f), reverb (0.5f, 0.18f), comp (-18.0f, 3.0f, 2.0f), {
            { "a_wt", 4 }, { "a_pos", 0.75f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.75f }, { "b_oct", 1 }, { "b_level", 0.25f },
            { "e1_a", 0.001f }, { "e1_d", 0.3f }, { "e1_s", 0.3f }, { "e1_r", 0.15f },
            { "f_type", 2 }, { "f_cutoff", 7000 }, { "f_res", 0.1f } } }));

        add (F, "Big Rave Stab", join ({ dist (0, 0.3f, 0.4f), reverb (0.75f, 0.3f), delay (6, 0.3f, 0.18f), {
            { "chord", 1 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.55f }, { "a_uni", 7 }, { "a_det", 0.5f }, { "a_blend", 0.85f },
            { "b_on", 1 }, { "b_wt", 9 }, { "b_pos", 0.4f }, { "b_uni", 3 }, { "b_det", 0.3f }, { "b_level", 0.45f },
            { "f_cutoff", 2500 }, { "f_res", 0.2f }, { "f_env", 0.35f }, { "e1_a", 0.001f }, { "e1_d", 0.4f }, { "e1_s", 0 },
            { "e1_r", 0.35f } } }));

        add (F, "Chord Memory Stab", join ({ delay (6, 0.35f, 0.22f), reverb (0.6f, 0.22f), dist (2, 0.15f, 0.4f), {
            { "chord", 3 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.62f }, { "a_uni", 2 }, { "a_det", 0.12f },
            { "f_cutoff", 1100 }, { "f_res", 0.3f }, { "f_env", 0.45f }, { "e2_d", 0.22f }, { "e2_s", 0.05f },
            { "e1_d", 0.32f }, { "e1_s", 0 }, { "e1_r", 0.28f } } }));

        add (F, "Main Room Lead", join ({ vibrato (1, 0.006f, 0.45f), delay (6, 0.35f, 0.2f), reverb (0.7f, 0.25f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 8 }, { "a_det", 0.45f },
            { "b_on", 1 }, { "b_wt", 2 }, { "b_pos", 0.9f }, { "b_oct", 1 }, { "b_uni", 4 }, { "b_det", 0.3f }, { "b_level", 0.45f },
            { "f_type", 3 }, { "f_cutoff", 6000 }, { "f_res", 0.15f }, { "f_key", 0.3f },
            { "e1_a", 0.004f }, { "e1_d", 0.6f }, { "e1_s", 0.75f }, { "e1_r", 0.35f }, { "mode", 2 }, { "glide", 0.03f } } }));

        add (F, "Pitch Drop Bass", join ({ monoBass (0.0f), subOn (0.5f), punch (1, 0.5f, 0.12f), dist (2, 0.25f, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.62f }, { "a_rand", 0 }, { "f_cutoff", 300 }, { "f_res", 0.35f }, { "f_drive", 0.4f },
            { "f_env", 0.4f }, { "e2_d", 0.2f }, { "e1_s", 0.8f } } }));

        add (F, "Uplift Pad", join ({ mod (1, 1, 12, 0.55f), chorus (0.35f), reverb (0.85f, 0.35f), delay (9, 0.3f, 0.15f), {
            { "a_wt", 2 }, { "a_pos", 0.8f }, { "a_uni", 7 }, { "a_det", 0.35f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_oct", 1 }, { "b_uni", 3 }, { "b_det", 0.2f }, { "b_level", 0.35f },
            { "f_type", 3 }, { "f_cutoff", 900 }, { "f_res", 0.2f }, { "e2_a", 3.0f }, { "e2_d", 1.0f }, { "e2_s", 1.0f }, { "e2_r", 2.0f },
            { "e1_a", 1.5f }, { "e1_d", 1.0f }, { "e1_s", 0.9f }, { "e1_r", 2.5f } } }));

        add (F, "Hoover Rave", join ({ mod (1, 3, 4, 0.3f), chorus (0.5f, 0.6f), dist (0, 0.2f, 0.5f), {
            { "a_wt", 1 }, { "a_pos", 0.55f }, { "a_uni", 7 }, { "a_det", 0.55f }, { "l1_sync", 0 }, { "l1_rate", 0.8f },
            { "m2_src", 2 }, { "m2_dst", 3 }, { "m2_amt", -0.1f }, { "e3_a", 0.0005f }, { "e3_d", 0.25f }, { "e3_s", 0 },
            { "mode", 2 }, { "glide", 0.1f }, { "f_type", 2 }, { "f_cutoff", 4000 }, { "f_res", 0.2f },
            { "e1_s", 0.9f }, { "e1_r", 0.3f } } }));

        add (F, "Piano Stab House", join ({ reverb (0.55f, 0.2f), comp (-18.0f, 3.0f, 2.0f), {
            { "chord", 3 }, { "voices", 16 }, { "a_wt", 7 }, { "a_pos", 0.6f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f },
            { "b_level", 0.2f }, { "e1_a", 0.001f }, { "e1_d", 1.0f }, { "e1_s", 0 }, { "e1_r", 0.4f },
            { "f_type", 2 }, { "f_cutoff", 5000 }, { "f_key", 0.4f }, { "vel", 0.7f } } }));

        add (F, "Pumping Saw Chords", join ({ reverb (0.6f, 0.25f), {
            { "chord", 5 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 5 }, { "a_det", 0.3f },
            { "f_cutoff", 2000 }, { "f_res", 0.15f }, { "e1_a", 0.005f }, { "e1_d", 0.5f }, { "e1_s", 0.8f }, { "e1_r", 0.4f },
            { "pump_on", 1 }, { "pump_div", 2 }, { "pump_depth", 0.7f }, { "pump_curve", 0.5f } } }));

        // =====================================================================
        //  DEFECTED STYLE - soulful house, deep chords, keys, strings
        // =====================================================================
        F = "Defected Style";
        add (F, "Deep House Chords", join ({ chorus (0.3f), delay (6, 0.3f, 0.18f), reverb (0.65f, 0.25f), {
            { "chord", 5 }, { "voices", 16 }, { "a_wt", 2 }, { "a_pos", 0.5f }, { "a_uni", 2 }, { "a_det", 0.1f },
            { "b_on", 1 }, { "b_wt", 7 }, { "b_pos", 0.3f }, { "b_level", 0.35f }, { "f_type", 3 }, { "f_cutoff", 1500 },
            { "f_res", 0.15f }, { "f_env", 0.3f }, { "e2_d", 0.5f }, { "e2_s", 0.4f },
            { "e1_a", 0.003f }, { "e1_d", 0.8f }, { "e1_s", 0.5f }, { "e1_r", 0.6f } } }));

        add (F, "Organ Bass", join ({ monoBass (0.01f), subOn (0.6f), comp (-18.0f, 3.0f, 2.0f), {
            { "a_wt", 4 }, { "a_pos", 0.25f }, { "a_rand", 0 }, { "e1_d", 0.4f }, { "e1_s", 0.6f }, { "e1_r", 0.08f },
            { "f_type", 2 }, { "f_cutoff", 1200 }, { "f_res", 0.1f } } }));

        add (F, "Soul Keys", join ({ mod (1, 5, 4, 0.35f), mod (2, 3, 18, 0.3f), chorus (0.35f, 0.25f, 1.2f), reverb (0.55f, 0.22f), {
            { "a_wt", 7 }, { "a_pos", 0.4f }, { "e1_a", 0.002f }, { "e1_d", 2.5f }, { "e1_s", 0.35f }, { "e1_r", 0.8f },
            { "f_type", 2 }, { "f_cutoff", 4000 }, { "f_key", 0.3f }, { "l1_sync", 0 }, { "l1_rate", 4.0f }, { "l1_mode", 0 },
            { "vel", 0.75f } } }));

        add (F, "Disco Strings", join ({ vibrato (1, 0.005f, 0.3f), chorus (0.5f, 0.5f), reverb (0.7f, 0.3f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 6 }, { "a_det", 0.3f },
            { "b_on", 1 }, { "b_wt", 1 }, { "b_pos", 0.2f }, { "b_uni", 4 }, { "b_det", 0.2f }, { "b_level", 0.5f },
            { "f_type", 3 }, { "f_cutoff", 2500 }, { "f_res", 0.1f },
            { "e1_a", 0.25f }, { "e1_d", 0.5f }, { "e1_s", 0.9f }, { "e1_r", 0.9f } } }));

        add (F, "Warm Analog Pad", join ({ mod (1, 3, 4, 0.3f), chorus (0.4f), reverb (0.75f, 0.3f), {
            { "a_wt", 1 }, { "a_pos", 0.3f }, { "a_uni", 4 }, { "a_det", 0.2f }, { "l1_sync", 0 }, { "l1_rate", 0.4f },
            { "l1_shape", 1 }, { "l1_mode", 0 }, { "b_on", 1 }, { "b_wt", 2 }, { "b_pos", 0.5f }, { "b_uni", 3 },
            { "b_fine", 6 }, { "b_level", 0.45f }, { "f_cutoff", 1800 }, { "f_res", 0.15f },
            { "e1_a", 0.8f }, { "e1_d", 1.0f }, { "e1_s", 0.85f }, { "e1_r", 1.8f }, { "analog", 0.6f } } }));

        add (F, "Garage Sub", join ({ legatoBass (0.08f), punch (1, 0.08f, 0.03f), dist (0, 0.2f, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.0f }, { "a_rand", 0 }, { "a_warp", 2 }, { "a_warpamt", 0.25f }, { "f_on", 0 },
            { "e1_a", 0.002f }, { "e1_d", 0.5f }, { "e1_s", 0.8f }, { "e1_r", 0.1f } } }));

        add (F, "Jackin Stab", join ({ delay (5, 0.3f, 0.2f), reverb (0.5f, 0.2f), {
            { "chord", 3 }, { "voices", 16 }, { "a_wt", 4 }, { "a_pos", 0.6f }, { "e1_d", 0.25f }, { "e1_s", 0 }, { "e1_r", 0.2f },
            { "f_type", 2 }, { "f_cutoff", 3500 }, { "f_env", 0.3f }, { "e2_d", 0.1f } } }));

        add (F, "Sunrise Pluck", join ({ mod (1, 1, 4, 0.4f), delay (6, 0.4f, 0.25f), reverb (0.7f, 0.3f), {
            { "a_wt", 11 }, { "a_pos", 0.6f }, { "a_uni", 3 }, { "a_det", 0.15f }, { "e2_d", 0.3f },
            { "f_type", 3 }, { "f_cutoff", 2500 }, { "f_res", 0.2f }, { "f_env", 0.3f },
            { "e1_d", 0.6f }, { "e1_s", 0 }, { "e1_r", 0.5f } } }));

        add (F, "Smooth Lead", join ({ vibrato (1, 0.006f, 0.5f), delay (9, 0.3f, 0.18f), reverb (0.6f, 0.25f), {
            { "a_wt", 0 }, { "a_pos", 0.3f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.1f }, { "b_oct", 1 }, { "b_level", 0.3f },
            { "f_type", 2 }, { "f_cutoff", 2800 }, { "f_res", 0.2f }, { "e1_a", 0.02f }, { "e1_d", 0.5f }, { "e1_s", 0.8f },
            { "e1_r", 0.4f }, { "mode", 2 }, { "glide", 0.1f } } }));

        add (F, "Funky Clav", join ({ mod (1, 5, 12, 0.3f), {
            { "a_wt", 1 }, { "a_pos", 0.6f }, { "f_type", 6 }, { "f_cutoff", 900 }, { "f_res", 0.25f }, { "f_env", 0.45f },
            { "f_drive", 0.5f }, { "eq_on", 1 }, { "eq_mid", 4 }, { "eq_midf", 1500 },
            { "e2_d", 0.12f }, { "e2_s", 0 }, { "e1_a", 0.001f }, { "e1_d", 0.25f }, { "e1_s", 0 }, { "e1_r", 0.1f },
            { "pha_on", 1 }, { "pha_rate", 0.5f }, { "pha_mix", 0.3f }, { "vel", 0.8f } } }));

        // =====================================================================
        //  LA PERA STYLE - minimal, quirky, rolling
        // =====================================================================
        F = "La Pera Style";
        add (F, "Minimal Bleep", join ({ delay (3, 0.5f, 0.3f), reverb (0.5f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.0f }, { "a_rand", 0 }, { "m1_src", 2 }, { "m1_dst", 1 }, { "m1_amt", 0.3f },
            { "e3_a", 0.0005f }, { "e3_d", 0.01f }, { "e3_s", 0 }, { "e1_a", 0.001f }, { "e1_d", 0.08f }, { "e1_s", 0 },
            { "e1_r", 0.06f }, { "f_on", 0 } } }));

        add (F, "Rolling Mono Bass", join ({ monoBass (0.02f), subOn (0.5f), mod (1, 3, 12, 0.08f), punch (2, 0.08f, 0.015f), {
            { "a_wt", 0 }, { "a_pos", 0.45f }, { "a_rand", 0 }, { "l1_div", 2 }, { "l1_shape", 3 },
            { "f_cutoff", 240 }, { "f_res", 0.4f }, { "f_env", 0.4f }, { "e2_d", 0.12f }, { "e2_s", 0.1f },
            { "e1_d", 0.2f }, { "e1_s", 0.5f }, { "e1_r", 0.05f } } }));

        add (F, "Quirky Perc Synth", join ({ mod (1, 9, 4, 0.5f), delay (3, 0.3f, 0.25f), reverb (0.45f, 0.15f), {
            { "a_wt", 9 }, { "a_pos", 0.3f }, { "m2_src", 2 }, { "m2_dst", 1 }, { "m2_amt", 0.3f },
            { "e3_a", 0.0005f }, { "e3_d", 0.02f }, { "e3_s", 0 }, { "e1_a", 0.001f }, { "e1_d", 0.1f }, { "e1_s", 0 },
            { "e1_r", 0.08f }, { "f_type", 4 }, { "f_cutoff", 300 } } }));

        add (F, "Glitch Pluck", join ({ mod (1, 3, 4, 0.5f), dist (4, 0.4f, 0.5f), delay (2, 0.3f, 0.2f), {
            { "a_wt", 10 }, { "a_pos", 0.2f }, { "l1_shape", 5 }, { "l1_div", 0 },
            { "e1_a", 0.001f }, { "e1_d", 0.18f }, { "e1_s", 0 }, { "e1_r", 0.12f },
            { "f_type", 2 }, { "f_cutoff", 3500 }, { "f_env", 0.2f } } }));

        add (F, "Wobble Groove", join ({ monoBass (0.03f), subOn (0.5f), mod (1, 3, 12, 0.35f), {
            { "a_wt", 0 }, { "a_pos", 0.6f }, { "a_rand", 0 }, { "l1_div", 4 }, { "l1_shape", 0 },
            { "f_cutoff", 300 }, { "f_res", 0.55f }, { "f_drive", 0.3f }, { "e1_s", 0.9f } } }));

        add (F, "Micro Stab", join ({ delay (6, 0.45f, 0.25f), reverb (0.55f, 0.2f), {
            { "chord", 7 }, { "voices", 16 }, { "a_wt", 6 }, { "a_pos", 0.55f },
            { "e1_a", 0.001f }, { "e1_d", 0.12f }, { "e1_s", 0 }, { "e1_r", 0.1f },
            { "f_type", 3 }, { "f_cutoff", 2000 }, { "f_res", 0.3f } } }));

        add (F, "Shaker Noise Synth", join ({ mod (1, 9, 12, 0.1f), reverb (0.35f, 0.1f), {
            { "a_on", 0 }, { "noise_on", 1 }, { "noise_type", 0 }, { "noise_level", 0.8f },
            { "f_type", 5 }, { "f_cutoff", 6000 }, { "f_res", 0.2f },
            { "e1_a", 0.001f }, { "e1_d", 0.06f }, { "e1_s", 0 }, { "e1_r", 0.05f }, { "vel", 0.8f } } }));

        add (F, "Bouncing Sync", join ({ mod (1, 1, 10, 0.6f), dist (0, 0.2f, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_warp", 1 }, { "a_warpamt", 0.2f }, { "e2_d", 0.2f }, { "e2_s", 0 },
            { "f_cutoff", 2500 }, { "f_res", 0.2f }, { "e1_d", 0.25f }, { "e1_s", 0.3f }, { "e1_r", 0.1f },
            { "mode", 2 }, { "glide", 0.02f } } }));

        add (F, "Deep Mallet", join ({ mod (1, 1, 4, 0.4f), reverb (0.6f, 0.25f), delay (6, 0.3f, 0.18f), {
            { "a_wt", 7 }, { "a_pos", 0.5f }, { "e2_d", 0.15f }, { "e1_a", 0.001f }, { "e1_d", 0.5f }, { "e1_s", 0 }, { "e1_r", 0.4f },
            { "f_type", 2 }, { "f_cutoff", 2500 } } }));

        add (F, "Hypno Arp Bass", join ({ monoBass (0.0f), subOn (0.45f), mod (1, 3, 4, 0.3f), {
            { "a_wt", 1 }, { "a_pos", 0.4f }, { "a_rand", 0 }, { "l1_div", 12 }, { "l1_mode", 0 },
            { "f_cutoff", 400 }, { "f_res", 0.4f }, { "f_env", 0.35f }, { "e2_d", 0.15f },
            { "e1_d", 0.22f }, { "e1_s", 0.2f }, { "e1_r", 0.06f } } }));

        // =====================================================================
        //  TECH HOUSE PRO - production-ready workhorses
        // =====================================================================
        F = "Tech House Pro";
        add (F, "TH Rubber Sub", join ({ monoBass (0.02f), subOn (0.6f), punch (1, 0.1f, 0.02f), mod (2, 10, 12, 0.4f), {
            { "a_wt", 0 }, { "a_pos", 0.4f }, { "a_rand", 0 }, { "f_cutoff", 140 }, { "f_res", 0.55f }, { "f_drive", 0.35f },
            { "f_env", 0.45f }, { "e2_d", 0.13f }, { "e2_s", 0.05f } } }));

        add (F, "TH Gritty Mid Bass", join ({ monoBass (0.02f), subOn (0.5f), dist (2, 0.45f, 0.7f), {
            { "a_wt", 8 }, { "a_pos", 0.45f }, { "a_rand", 0 }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_oct", -1 },
            { "b_level", 0.4f }, { "b_rand", 0 }, { "f_cutoff", 700 }, { "f_res", 0.3f }, { "f_drive", 0.6f }, { "f_env", 0.35f },
            { "e1_s", 0.8f }, { "eq_on", 1 }, { "eq_low", -2 }, { "eq_mid", 3 }, { "eq_midf", 900 } } }));

        add (F, "TH FM Bass", join ({ monoBass (0.02f), subOn (0.45f), mod (1, 1, 10, 0.4f), {
            { "a_wt", 0 }, { "a_pos", 0.0f }, { "a_rand", 0 }, { "a_warp", 4 }, { "a_warpamt", 0.5f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.0f }, { "b_level", 0.0f }, { "b_rand", 0 },
            { "e2_d", 0.15f }, { "e2_s", 0 }, { "f_cutoff", 2000 }, { "f_res", 0.1f }, { "e1_s", 0.8f }, { "e1_r", 0.06f } } }));

        add (F, "TH Pluck Classic", join ({ delay (6, 0.35f, 0.22f), reverb (0.55f, 0.2f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 3 }, { "a_det", 0.18f },
            { "f_cutoff", 450 }, { "f_res", 0.4f }, { "f_env", 0.6f }, { "e2_d", 0.2f }, { "e2_s", 0 },
            { "e1_d", 0.35f }, { "e1_s", 0 }, { "e1_r", 0.3f } } }));

        add (F, "TH Dub Chord", join ({ delay (6, 0.55f, 0.35f, 0.35f), reverb (0.75f, 0.3f, 0.7f), {
            { "chord", 9 }, { "voices", 16 }, { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 2 }, { "a_det", 0.08f },
            { "f_cutoff", 650 }, { "f_res", 0.35f }, { "f_env", 0.4f }, { "e2_d", 0.25f }, { "e2_s", 0.1f },
            { "e1_d", 0.45f }, { "e1_s", 0 }, { "e1_r", 0.35f }, { "analog", 0.5f } } }));

        add (F, "TH Stab Minor7", join ({ reverb (0.6f, 0.25f), delay (6, 0.3f, 0.2f), {
            { "chord", 3 }, { "voices", 16 }, { "a_wt", 5 }, { "a_pos", 0.45f }, { "a_uni", 2 }, { "a_det", 0.12f },
            { "f_type", 3 }, { "f_cutoff", 1600 }, { "f_res", 0.25f }, { "f_env", 0.35f }, { "e2_d", 0.15f },
            { "e1_d", 0.22f }, { "e1_s", 0 }, { "e1_r", 0.2f } } }));

        add (F, "TH Vocal Lead", join ({ mod (1, 10, 4, 0.5f), mod (2, 3, 4, 0.15f), dist (2, 0.2f, 0.4f), delay (6, 0.3f, 0.2f),
                                          reverb (0.6f, 0.2f), {
            { "a_wt", 3 }, { "a_pos", 0.5f }, { "l1_sync", 0 }, { "l1_rate", 3.0f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_level", 0.25f },
            { "f_type", 2 }, { "f_cutoff", 4000 }, { "f_res", 0.2f }, { "mode", 2 }, { "glide", 0.05f },
            { "e1_s", 0.85f }, { "e1_r", 0.2f } } }));

        add (F, "TH Acid Line", join ({ legatoBass (0.06f), mod (1, 5, 12, 0.3f), mod (2, 10, 12, 0.5f), dist (2, 0.4f, 0.6f),
                                         delay (3, 0.3f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.75f }, { "a_rand", 0 }, { "f_cutoff", 320 }, { "f_res", 0.82f }, { "f_drive", 0.5f },
            { "f_env", 0.55f }, { "e2_d", 0.18f }, { "e2_s", 0 }, { "e1_d", 0.25f }, { "e1_s", 0.7f }, { "e1_r", 0.05f },
            { "vel", 0.4f } } }));

        add (F, "TH Reese", join ({ legatoBass (0.05f), subOn (0.5f), mod (1, 3, 12, 0.1f), dist (2, 0.3f, 0.5f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_uni", 2 }, { "a_det", 0.55f }, { "a_blend", 1.0f }, { "a_width", 0.3f },
            { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.5f }, { "b_uni", 2 }, { "b_det", 0.7f }, { "b_oct", -1 }, { "b_level", 0.55f },
            { "b_width", 0.3f }, { "f_cutoff", 500 }, { "f_res", 0.25f }, { "f_drive", 0.4f }, { "l1_div", 12 }, { "l1_mode", 0 },
            { "e1_s", 1.0f } } }));

        add (F, "TH Organ Stab Bright", join ({ chorus (0.35f, 0.4f, 0.7f), comp (-18.0f, 3.0f, 2.0f), reverb (0.5f, 0.15f), {
            { "a_wt", 4 }, { "a_pos", 1.0f }, { "e1_a", 0.001f }, { "e1_d", 0.3f }, { "e1_s", 0.2f }, { "e1_r", 0.15f },
            { "f_type", 2 }, { "f_cutoff", 8000 } } }));

        add (F, "TH Noise Riser", join ({ mod (1, 3, 18, 0.5f), reverb (0.9f, 0.45f), delay (6, 0.35f, 0.2f), {
            { "a_on", 0 }, { "noise_on", 1 }, { "noise_type", 0 }, { "noise_level", 0.9f }, { "l1_sync", 0 }, { "l1_rate", 0.4f },
            { "f_type", 3 }, { "f_cutoff", 300 }, { "f_res", 0.45f }, { "f_env", 0.9f }, { "e2_a", 8.0f }, { "e2_s", 1.0f },
            { "e2_r", 3.0f }, { "e1_a", 7.0f }, { "e1_s", 1.0f }, { "e1_r", 3.0f }, { "mode", 1 } } }));

        add (F, "TH Downlifter", join ({ mod (1, 1, 1, 0.5f), reverb (0.9f, 0.45f), {
            { "a_wt", 9 }, { "a_pos", 1.0f }, { "a_level", 0.4f }, { "a_uni", 4 }, { "a_det", 0.4f },
            { "noise_on", 1 }, { "noise_level", 0.7f }, { "f_type", 3 }, { "f_cutoff", 400 }, { "f_res", 0.3f }, { "f_env", 0.8f },
            { "e2_a", 0.001f }, { "e2_d", 4.0f }, { "e2_s", 0 }, { "e2_r", 1.0f },
            { "e1_a", 0.01f }, { "e1_d", 4.0f }, { "e1_s", 0 }, { "e1_r", 1.0f }, { "mode", 1 } } }));

        add (F, "TH Atmos Pad", join ({ mod (1, 3, 4, 0.4f), reverb (0.95f, 0.45f), delay (9, 0.35f, 0.2f), {
            { "a_wt", 10 }, { "a_pos", 0.3f }, { "a_uni", 6 }, { "a_det", 0.3f }, { "l1_sync", 0 }, { "l1_rate", 0.05f }, { "l1_mode", 0 },
            { "b_on", 1 }, { "b_wt", 3 }, { "b_pos", 0.6f }, { "b_uni", 4 }, { "b_level", 0.4f }, { "b_det", 0.3f },
            { "f_type", 3 }, { "f_cutoff", 1800 }, { "f_res", 0.2f }, { "e1_a", 2.5f }, { "e1_d", 2.0f }, { "e1_s", 0.9f },
            { "e1_r", 4.0f }, { "noise_on", 1 }, { "noise_type", 1 }, { "noise_level", 0.15f },
            { "pha_on", 1 }, { "pha_rate", 0.08f }, { "pha_mix", 0.35f } } }));

        add (F, "TH Bell Lead", join ({ delay (6, 0.4f, 0.25f), reverb (0.65f, 0.3f), {
            { "a_wt", 7 }, { "a_pos", 0.85f }, { "b_on", 1 }, { "b_wt", 0 }, { "b_pos", 0.0f }, { "b_oct", 2 }, { "b_level", 0.15f },
            { "e1_a", 0.001f }, { "e1_d", 1.2f }, { "e1_s", 0.1f }, { "e1_r", 0.6f }, { "f_type", 2 }, { "f_cutoff", 6000 },
            { "f_key", 0.3f } } }));

        add (F, "TH Sub 808 Glide", join ({ legatoBass (0.12f), punch (1, 0.12f, 0.04f), dist (0, 0.3f, 0.5f, 0.3f), {
            { "a_wt", 0 }, { "a_pos", 0.0f }, { "a_rand", 0 }, { "a_warp", 2 }, { "a_warpamt", 0.15f }, { "f_on", 0 },
            { "e1_a", 0.001f }, { "e1_d", 1.2f }, { "e1_s", 0.5f }, { "e1_r", 0.15f } } }));

        add (F, "TH Percussive Zap", join ({ reverb (0.4f, 0.15f), delay (2, 0.3f, 0.15f), {
            { "a_wt", 0 }, { "a_pos", 0.5f }, { "a_rand", 0 }, { "m1_src", 2 }, { "m1_dst", 1 }, { "m1_amt", 1.0f },
            { "e3_a", 0.0005f }, { "e3_d", 0.03f }, { "e3_s", 0 }, { "e1_a", 0.001f }, { "e1_d", 0.1f }, { "e1_s", 0 },
            { "e1_r", 0.08f }, { "f_type", 4 }, { "f_cutoff", 150 } } }));

        add (F, "TH Organ Bass", join ({ monoBass (0.015f), subOn (0.55f), comp (-18.0f, 3.0f, 2.0f), {
            { "a_wt", 4 }, { "a_pos", 0.35f }, { "a_rand", 0 }, { "f_cutoff", 900 }, { "f_res", 0.2f }, { "f_env", 0.3f },
            { "e2_d", 0.2f }, { "e1_d", 0.3f }, { "e1_s", 0.6f }, { "e1_r", 0.06f } } }));

        add (F, "TH Bleep Sequence", join ({ mod (1, 9, 4, 0.4f), delay (3, 0.5f, 0.3f), reverb (0.5f, 0.2f), {
            { "a_wt", 11 }, { "a_pos", 0.2f }, { "e1_a", 0.001f }, { "e1_d", 0.12f }, { "e1_s", 0 }, { "e1_r", 0.1f },
            { "f_type", 2 }, { "f_cutoff", 5000 } } }));

        add (F, "TH Pumping Pad", join ({ reverb (0.7f, 0.3f), {
            { "chord", 3 }, { "voices", 16 }, { "a_wt", 2 }, { "a_pos", 0.6f }, { "a_uni", 5 }, { "a_det", 0.3f },
            { "f_type", 3 }, { "f_cutoff", 2200 }, { "e1_a", 0.4f }, { "e1_s", 0.9f }, { "e1_r", 1.0f },
            { "pump_on", 1 }, { "pump_div", 2 }, { "pump_depth", 0.85f }, { "pump_curve", 0.6f } } }));

        // ---------------------------------------------------------------------
        //  Level calibration: master volume per preset so everything lands at a
        //  similar short-term loudness with peaks under -1 dBFS (measured by
        //  Tests/RenderTest --calibrate).  Risers measured over their full 8 s rise.
        // ---------------------------------------------------------------------
        static const std::pair<const char*, float> levels[] = {
            { "Init", -4.6f },
            { "Rolling Sub Groove", -4.6f },
            { "Warm Rubber Bass", -3.2f },
            { "Mood Pluck", -2.5f },
            { "Late Night Keys", -6.7f },
            { "Hypno Bleep", -4.4f },
            { "Velvet Pad", -5.6f },
            { "Swing Stab", -3.5f },
            { "Bouncy Mid Bass", -1.7f },
            { "Wonky Lead", 1.1f },
            { "Afterglow Chords", -7.9f },
            { "Driving Reese", -5.7f },
            { "Rumble Low End", -12.8f },
            { "Acid Groove 303", -2.6f },
            { "Hypnotic Sequence", -3.3f },
            { "Dark Stab", -3.7f },
            { "Warehouse Lead", -10.8f },
            { "Tension Riser", 2.0f },
            { "Deep Pulse Bass", -2.3f },
            { "Metal Perc Synth", -2.0f },
            { "Tunnel Pad", -3.1f },
            { "Funk Slap Bass", 1.9f },
            { "Talkbox Lead", 2.2f },
            { "Organ Hit", -2.6f },
            { "Bubble Bass", -0.8f },
            { "Groove Pluck", -0.8f },
            { "Disco Rubber", -2.3f },
            { "Vowel Chop", -3.4f },
            { "Bright Brass Stab", -6.7f },
            { "Sunset Keys", -6.1f },
            { "Party Lead", -5.3f },
            { "Rubber Bass Classic", -4.5f },
            { "Classic House Organ", 0.0f },
            { "Big Rave Stab", -2.9f },
            { "Chord Memory Stab", -3.9f },
            { "Main Room Lead", -5.6f },
            { "Pitch Drop Bass", -4.2f },
            { "Uplift Pad", -10.5f },
            { "Hoover Rave", -0.4f },
            { "Piano Stab House", -2.5f },
            { "Pumping Saw Chords", -7.5f },
            { "Deep House Chords", -8.9f },
            { "Organ Bass", 1.1f },
            { "Soul Keys", -6.0f },
            { "Disco Strings", -8.5f },
            { "Warm Analog Pad", -7.7f },
            { "Garage Sub", -3.8f },
            { "Jackin Stab", -6.1f },
            { "Sunrise Pluck", -6.4f },
            { "Smooth Lead", -5.8f },
            { "Funky Clav", 5.6f },
            { "Minimal Bleep", -5.5f },
            { "Rolling Mono Bass", 2.2f },
            { "Quirky Perc Synth", -6.2f },
            { "Glitch Pluck", 5.2f },
            { "Wobble Groove", -2.9f },
            { "Micro Stab", 3.8f },
            { "Shaker Noise Synth", -2.9f },
            { "Bouncing Sync", 6.0f },
            { "Deep Mallet", -4.2f },
            { "Hypno Arp Bass", 5.2f },
            { "TH Rubber Sub", -6.2f },
            { "TH Gritty Mid Bass", -5.8f },
            { "TH FM Bass", -0.4f },
            { "TH Pluck Classic", -3.8f },
            { "TH Dub Chord", -3.2f },
            { "TH Stab Minor7", -4.0f },
            { "TH Vocal Lead", -4.0f },
            { "TH Acid Line", -4.0f },
            { "TH Reese", -5.5f },
            { "TH Organ Stab Bright", -0.4f },
            { "TH Noise Riser", -1.2f },
            { "TH Downlifter", 0.9f },
            { "TH Atmos Pad", -4.2f },
            { "TH Bell Lead", -8.0f },
            { "TH Sub 808 Glide", -4.3f },
            { "TH Percussive Zap", -5.0f },
            { "TH Organ Bass", 1.5f },
            { "TH Bleep Sequence", -3.7f },
            { "TH Pumping Pad", -10.5f },
        };
        for (auto& preset : b)
            for (auto& [name, db] : levels)
                if (preset.name == name)
                    preset.values.push_back ({ "master", db });

        return b;
    }
}

const std::vector<FactoryPreset>& getFactoryPresets()
{
    static const std::vector<FactoryPreset> bank = build();
    return bank;
}
