# Aleatoric Setup

*Note: this is still a work in progress, details may change.*

The sequencer provides a small scripting DSL with Scheme-inspired syntax.

The goals are generating music from scratch relying on dice rolls and some underlying ideas, or writing custom modifiers for e.g. adding random variations to an existing piece.

The non-goals are anything related to audio processing or sound generation, or extending the UI. This tool is made with focus on algorithmic composition in mind, it derives a lot of inspiration from [Common Music](https://commonmusic.sourceforge.net), except I had hard time making sense of their code examples, so trying to implement my own take to generating music [aleatorically](https://wikipedia.org/wiki/Aleatoric_music).

### Oh no, another lisp

The syntax was more of an artistic choice for the sake of [timeless vibes](https://xkcd.com/297/), but under the hood it's a toy language that tries to be easier to read (no car/cdr/cddadr/cadddr) and encourages an old-school fp style (data is mostly immutable, functions are mostly pure, use map/filter/reduce instead of for loops), which is again for the sake of timeless vibes.

Here will be a short overview for details, but the code examples ([(1)](#example-1), [(2)](#example-2)) are the best way to get a grasp of a language.

```lisp
; TODO: describe how it differs from Scheme and list the built-in symbols;
; will get to it someday (the language is still a subject to change),
; for now just move on to examples.
```

## Generating music from scratch

The `Control + ~` hotkey opens the script editor, and the `F5` hotkey in the editor runs the script in the current project. To get started, paste the example below in the editor and mouse-hover over symbols to see what they evaluate to in the sandbox.

Generating music from scratch with code seems like an overly ambitious goal because algorithms are rigid and only approximate your methods and thought process. But algorithms may serve as a way to escape tedious work and hopefully focus at higher level.

My guess is that it may be more practical to take the best of both worlds: have lots of generated thematic material based on some ideas and chance, and then refine and/or cull it manually in the sequencer, or use it as an inspiration. In the example below, I ended up just trying many different random seeds to choose more lucky ones.

### Example 1

This script is trying to mimic the distinctive dark fantasy style from Disciples II OST.

See the rendered examples in [this YouTube video](https://youtu.be/z_EVqsC3XoY).

```lisp
; this script is trying to copy the dark fantasy style
; from Disciples II soundtrack by Philippe Charron
; using the diatonic framework and an arbitrary temperament;

; I wrote down some scores by ear and noticed that the music jumps
; through different keys a lot, and it either takes a small step up or down,
; or finds a chord in the new key sharing a note with the previous chord;

; let's try to mimic this approach: build phrases out of these jumps,
; and connect phrases similarly: rhyming or transposing by a cent-sized step,
; the results are supposed to be very different but in the same spirit;

; this example lacks melody because I don't have good ideas how to do it,
; and the structure is way too random, but whatever, this is my first attempt,
; and hopefully it's good enough as a script demonstration;

; (seed 29243055) ; set a specific seed if you need it

; since chords jump around different keys all the time,
; start by generating a list of key jumps with chords:
; (key-offset (scale) (chord-degrees) chord-length),
; a list of those would be e.g. ((0 (2 2 1 2 2 2 1) (1 3 5) 4.0) ...);
; first, add some helpers for this ad-hoc data structure
(define (harmony:key h) (nth 0 h))
(define (harmony:scale h) (nth 1 h))
(define (harmony:chord h) (nth 2 h))
(define (harmony:length h) (nth 3 h))
(define (harmony:with-delta-key h delta)
  (map-indexed (λ (index x) (if (= index 0) (+ delta x) x)) h))
(define (harmony:with-chord h chord)
  (map-indexed (λ (index x) (if (= index 2) chord x)) h))
(define (harmony:with-length h length)
  (map-indexed (λ (index x) (if (= index 3) length x)) h))

(define bar-length 4)

; renders an in-scale key to a MIDI key wrapped within one period
(define (render-key root-key scale degree (base-period 4))
  (define chromatic-key (+ root-key (scale:render-key scale degree)))
  (define base-key (* project:period-size base-period))
  (+ base-key (modulo chromatic-key project:period-size)))

; by "rhyme" I mean a random different key and scale, such that
; a triad chord in it intersects with some previous chord
(define (find-random-rhyme new-scale from-harmony)
  (define from-key (harmony:key from-harmony))
  (define from-scale (harmony:scale from-harmony))
  (define from-chord (harmony:chord from-harmony))
  (define my-row (render-key 0 new-scale (random triad)))
  (define other-row (render-key from-key from-scale (random from-chord)))
  (define new-key (modulo (- other-row my-row) project:period-size))
  (list new-key new-scale triad bar-length))

; the original OST switches between major and minor all the time,
; this is for the "medieval" mood I'm guessing; let's define scales
(define scale-major (scale:find "Ionian"))
; use a tense minor for spooky vibes, Harmonic/Hungarian are my favourite here
(define scale-minor (scale:find "Hungarian Minor"))

(define (get-random-step-size)
  (define cent-ish (/ project:period-size 12)) ; a hack for larger equal temperaments
  (random (list cent-ish (* cent-ish -1) (+ cent-ish 1))))

; and let's do a lot of random key jumps:
; either reuse the same harmony, but step up or down for the uncertain mood,
; or find a random rhyme (with random scale (but prefer minor (it's dark fantasy)))
(define (key-jump from-harmony)
  (if (random d7)
    (harmony:with-delta-key from-harmony (get-random-step-size))
    (find-random-rhyme (if (random d4) scale-major scale-minor) from-harmony)))

; makes a jump to the new tonic followed by a mediant or submediant triad in the same key
(define (key-jump-and-echo from-harmony)
  (define new-harmony (key-jump from-harmony))
  (define alt-chord (random (list triad:mediant triad:submediant)))
  (define maybe-alt-harmony
    (if (random d3) new-harmony (harmony:with-chord new-harmony alt-chord)))
  (list new-harmony maybe-alt-harmony))

; these helpers will generate the harmonic context, all tracks will be built on top of it
(define (make-phrase from-harmony)
  (define a (key-jump-and-echo from-harmony))
  (define ab (key-jump-and-echo (last a)))
  (define bc (key-jump-and-echo (last ab)))
  (random (list
    (append a ab bc a)
    (append a ab bc bc)
    (append a ab bc ab)
    (append a a ab bc)
    (append a ab ab bc))))

; returns a phrase transposed to rhyme with something (or with itself (by default))
(define (transpose-to-rhyme phrase (from-harmony (last phrase)))
  (define to-harmony (first phrase))
  (define new-harmony
    (find-random-rhyme (harmony:scale to-harmony) from-harmony))
  (define new-key-delta
    (- (harmony:key new-harmony) (harmony:key to-harmony)))
  (map (λ (x) (harmony:with-delta-key x new-key-delta)) phrase))

(define (transpose-random-steps phrase)
  (define one-random-offset (get-random-step-size))
  (map (λ (x) (harmony:with-delta-key x one-random-offset)) phrase))

(define (make-echo phrase)
  (define echo-length (random 2 (length phrase)))
  (define echo (last (- echo-length (% echo-length 2)) phrase))
  (map (λ (x)
    (-> x
      (harmony:with-length (* bar-length (if (random d16) 3 1)))
      (harmony:with-chord (random (list
        triad
        sus2
        (list supertonic mediant dominant)
        (list tonic mediant submediant))))))
    echo))

(define (variate phrase (from-harmony (last phrase)))
  (if (random d8)
    (transpose-random-steps (make-echo phrase))
    (transpose-to-rhyme (make-echo phrase) from-harmony)))

(define (simplify phrase)
  (map-indexed (λ (i x) (if (< i 4) (harmony:with-chord x triad) x)) phrase))

; generate the harmonic context
; (this is my least favourite part of the script,
; probably the piece structure should have been coded explicitly instead,
; it's too random and often generates crap, so I had to pick lucky seeds
; (but it's probably good enough to demonstrate the language))
(define harmony-base
  (begin
    (define starting-key (random 0 (/ project:period-size 2)))
    (define starting-harmony (list starting-key scale-minor triad bar-length))
    (define phrase (make-phrase starting-harmony))
    (define sentence
      (reduce
        (λ (result n)
          (define next-phrase (append phrase (variate phrase)))
          (append result (if (random d7)
            (transpose-random-steps next-phrase)
            (transpose-to-rhyme next-phrase (last result)))))
      ; start with the simplified motive (one of my favourite no-brainer tricks)
      (simplify phrase)
      (range 1 3)))
    (append sentence
      (->> sentence
        ; repeat the same thing, but adding more random uncertainty
        (map (λ (x)
          (define new-chord (random (list
            triad
            (list supertonic mediant dominant)
            (list mediant dominant submediant))))
          (-> x (harmony:with-length bar-length) (harmony:with-chord new-chord))))
        ; rhyme with itself (but it also means rhyming with the original sentence)
        (transpose-to-rhyme)
        ; this hopefully cuts the last half-phrase to leave it unresolved
        (first (- (length sentence) 2))
        ; and make the last jumps sound longer
        (map-indexed (λ (i x)
          (define is-ending (> i (- (length sentence) 4)))
          (if is-ending
            (-> x (harmony:with-length (* bar-length 4)) (harmony:with-chord triad))
            x)))))))

; update the project
(timeline:reset)

; add key signatures first
; (some refactoring methods used later (like refactor:arpeggiate)
; are stateful and depend on the harmonic context and time context)
(reduce
  (λ (current-beat harmony)
    (timeline:add-key current-beat (harmony:key harmony) (harmony:scale harmony))
    (+ current-beat (harmony:length harmony)))
  0
  harmony-base)

; voicing should've been done better (depending on the structure (maybe?)),
; but for demo purposes let's do phased fade-ins/outs and filter out the muted notes
(define phase-a (random 0 π))
(define phase-b (+ phase-a (/ π 2)))
(define phase-c (+ phase-a π))
(define (filter-out-muted min-velocity notes)
  (filter (λ (x) (> (last x) min-velocity)) notes))
(define (filter-out-short min-length notes)
  (filter (λ (x) (> (nth 2 x) min-length)) notes))

; this is a hack to get the last note's beat+length from a list of notes,
; the note format is (beat key length velocity), e.g. (list 0 69 4 0.5)
(define (get-last-beat notes)
  (if (empty notes) 0 (+ (nth 0 (last notes)) (nth 2 (last notes)))))

; (track:remove
;   "Contrabass" "Cellos" "Violas" "Violins" "Choir" "Brass" "Woodwinds")

; bass will be the simplest track: long notes, always at tonic
(track:make "Contrabass"
  (->> harmony-base
    (reduce
      (λ (result x)
        (define beat (get-last-beat result))
        (define key (render-key (harmony:key x) (harmony:scale x) tonic 2))
        (define note (list beat key (harmony:length x) 0.2))
        (+ result note))
      ())
    (refactor:join-adjacent)))

; violas and violins will play whatever chords we have in the harmonic context
; (lots of magic numbers below are hard-coded velocity ranges for my instruments)
(define (make-violins-base (min-velocity 0.2) (max-velocity 0.22))
  (reduce
    (λ (result x)
      (define beat (get-last-beat result))
      (define velocity-fade (* -0.004 (% beat 16)))
      (append result (map
        (λ (degree)
          (define key (render-key (harmony:key x) (harmony:scale x) degree 4))
          (define velocity (+ velocity-fade (random min-velocity max-velocity)))
          (define note (list beat key (harmony:length x) velocity))
          note)
        (harmony:chord x))))
    ()
    harmony-base))
(track:make "Violas"
  (->> (make-violins-base)
    (refactor:invert-chord -1)
    (refactor:join-adjacent)))
(track:make "Violins"
  (->> (make-violins-base 0.125 0.15)
    (refactor:invert-chord 2)
    (refactor:join-adjacent)
    ; shorter notes on violins sound annoying, get rid of them
    (filter-out-short bar-length)))

; cellos are going to sound like "taaa da" or "taa daa" on each bar
; (the first sound will be stable (tonic/dominant) and the second will be unstable,
; think of this code as of manual arpeggiation (see another arpeggiator example below))
(track:make "Cellos"
  (->> harmony-base
    (reduce
      (λ (result x)
        (define key (harmony:key x))
        (define scale (harmony:scale x))
        (define chord (harmony:chord x))
        (define beat (get-last-beat result))
        (define length-a (* (harmony:length x) (if (random d16) 0.75 0.5)))
        (define length-b (- (harmony:length x) length-a))
        (define degree-a (if (random d3) dominant tonic))
        (define degree-b (random (list degree-a supertonic mediant)))
        (define key-a (render-key key scale degree-a 3))
        (define key-b (render-key key scale degree-b 3))
        (define velocity-fade (* -0.002 (% beat 16)))
        (define note-a (list beat key-a length-a (+ velocity-fade 0.15)))
        (define note-b (list (+ beat length-a) key-b length-b (+ velocity-fade 0.1)))
        (+ result note-a note-b))
      ())
    (refactor:join-adjacent)
    (refactor:legato)))

(track:make "Choir"
  (->> harmony-base
    (reduce
      (λ (result x)
        (define length (harmony:length x))
        (define beat (get-last-beat result))
        (define key-a (render-key (harmony:key x) (harmony:scale x) tonic 4))
        (define key-b (render-key (harmony:key x) (harmony:scale x) dominant 5))
        (define key-c (render-key (harmony:key x) (harmony:scale x) mediant 4))
        (define key-d (render-key (harmony:key x) (harmony:scale x) dominant 4))
        (define ~ (/ (sin (+ phase-c (/ beat (* 2 π)))) 16))
        (define ~~ (/ (sin (+ phase-b (/ beat (* 2 π)))) 12))
        (define fade (* -0.004 (% beat 16)))
        (define note-a (list beat key-a length (+ ~ fade (random 0.025 0.05))))
        (define note-b (list beat key-b length (+ ~ fade (random 0.025 0.05))))
        (define note-c (list beat key-c length (+ ~~ fade (random 0.025 0.05))))
        (define note-d (list beat key-d length (+ ~~ fade (random 0.025 0.05))))
        (+ result note-a note-b note-c note-d))
      ())
    (refactor:join-adjacent)
    (filter-out-muted 0.05)))

; arpeggiators are defined in terms of in-scale notes
; and applied to a chord sequence in whatever harmonic context,
; the arpeggiator format is a list of (beat degree length velocity)
(define brass-arp (list
  (list 0 (+ 7 tonic) 4 0.1)
  (list 1 dominant 1 0.05)
  (list 2 mediant 1 0.1)
  (list 3 tonic 2 0.1)
  (list 5 dominant 1 0.1)
  (list 6 mediant 2 0.15)
  (list 7 tonic 1 0.1)))
(track:make "Brass"
  (->> harmony-base
    (reduce
      (λ (result x)
        (define key (harmony:key x))
        (define length (harmony:length x))
        (define scale (harmony:scale x))
        (define chord (harmony:chord x))
        (define beat (get-last-beat result))
        (define ~ (/ (sin (+ phase-c (/ beat π))) 20))
        (define key-a (render-key key scale (nth 0 chord) 4))
        (define key-b (render-key key scale (nth 1 chord) (if (random d3) 3 4)))
        (define key-c (render-key key scale (nth 2 chord) 3))
        (define note-a (list beat key-a length (+ ~ 0.05)))
        (define note-b (list beat key-b length (+ ~ 0.05)))
        (define note-c (list beat key-c length (+ ~ 0.05)))
        (+ result note-a note-b note-c))
      ())
    (refactor:arpeggiate brass-arp)
    (refactor:join-adjacent)
    (filter-out-muted 0.08)))

; two voices with wider intervals for the woodwinds
(track:make "Woodwinds"
  (->> harmony-base
    (reduce
      (λ (result x)
        (define chord (harmony:chord x))
        (define beat (get-last-beat result))
        (define key-a (render-key (harmony:key x) (harmony:scale x) (last chord) 5))
        (define key-b (render-key (harmony:key x) (harmony:scale x) (first chord) 5))
        (define ~ (/ (sin (+ phase-a (/ beat (* 2 π)))) 32))
        (define fade (* -0.005 (% beat 16)))
        (define note-a (list beat key-a (harmony:length x) (+ ~ fade (random 0.15 0.2))))
        (define note-b (list beat key-b (harmony:length x) (+ ~ fade (random 0.2 0.22))))
        (+ result note-a note-b))
      ())
    (refactor:join-adjacent)))
```

### Example 2

This script generates tracks with hard-coded structure. See the rendered examples [in this video](https://youtu.be/6Kv_5TGt4iQ).

```lisp
; let's try to make a spacesynth-inspired chiptune track
; with ZX Spectrum sound using wtbeep for all instruments:
; https://github.com/utz82/ZX-Spectrum-1-Bit-Routines/tree/master/wtbeep
; (SoundFont version here: https://shiru.untergrund.net/software.shtml)

; my recipe will be this: follow the structure of a pop song
; (e.g. intro - chorus - verse - chorus - bridge - chorus - etc),
; the chorus will be made out of some generic progressions in minor,
; and verses will try to be spacey by using major chords jumping over
; what they call chromatic mediants (e.g. a tritone or a third apart);
; is this a good idea? maybe not, let's do it anyway and see what happens

; -------------------------------------------------------------------
; structure & helpers

; shorthands for chords (not including scale but assuming using minor later):
(define i triad:tonic)
(define ii triad:supertonic)
(define iii triad:mediant)
(define iv triad:subdominant)
(define v triad:dominant)
(define v7 seventh:dominant)
(define vi triad:submediant)
(define vii triad:subtonic)

(define (make-progression-a)
  (list ii ii
    (random (list iv v v7))
    (random (list iii vii))))

(define (make-progression-b)
  (list ii
    (random (list ii ii iii vii))
    (random (list iv v v7))
    (random (list i iii))))

(define chorus-scale (scale:find "Dorian"))
(define chorus-key (+ (/ project:period-size 3) (random 0 7)))
(define verse-scale (scale:find "Mixolydian"))

; loosely following the definition in wikipedia.org/wiki/Chromatic_mediant
; (roots are related by a M3 or m3 (or in between in larger temperaments))
(define (pick-random-mediant (from-key 0))
  (define minor-3 (round (* project:period-size (/ 294.0 1200.0))))
  (define major-3 (round (* project:period-size (/ 407.0 1200.0))))
  (define key-delta (* (if (random d8) 1 -1) (random minor-3 (+ 1 major-3))))
  (modulo (+ from-key key-delta) project:period-size))

; makes a list of key jumps (the instruments will pick chords on their own)
(define (make-verse (from-key chorus-key))
  (define a from-key)
  (define b (pick-random-mediant a))
  (define c (pick-random-mediant b))
  (append
    (random (list (list a a b b) (list a b b c)))
    (random (list (list c c b a) (list c b b a)))))

(define (variate-chorus chorus)
  (define (maybe-suspended i x)
    (if (and (= i 1) (random d3)) (+ x (random '(-1 1))) x))
  (map (λ (chord) (map-indexed maybe-suspended chord)) chorus))

(define (simplify-chorus chorus)
  (map-indexed (λ (i x) (if (= i 3) (nth (- i 1) chorus) x)) chorus))

(define (make-intro chorus)
  (first (/ (length chorus) 2) (simplify-chorus chorus)))

(define (make-bridge chorus)
  (last (/ (length chorus) 2) chorus))

(define bar-length 4)

; append starting beats for later use:
; (key key key) -> ((beat key) (beat key) (beat key)), or
; (chord chord chord) -> ((beat chord) (beat chord) (beat chord))
(define (add-timestamps progression (from-bar 0) (chord-length bar-length))
  (map-indexed
    (λ (i x) (list (+ (* from-bar chord-length) (* i chord-length)) x))
    progression))

(define verse-base-a (make-verse))
(define verse-base-b (make-verse))
(define chorus-base-a (append (make-progression-a) (make-progression-b)))
(define chorus-base-b (append (make-progression-a) (make-progression-b)))

(define intro (-> chorus-base-a (make-intro) (add-timestamps)))
(define chorus-a (-> chorus-base-a (add-timestamps 4)))
(define chorus-b (-> chorus-base-b (add-timestamps 12)))
(define verse-a (-> verse-base-a (add-timestamps 20)))
(define chorus-c (-> chorus-base-b (variate-chorus) (add-timestamps 28)))
(define bridge (-> chorus-base-b (make-bridge) (add-timestamps 36)))
(define chorus-d (-> chorus-base-a (variate-chorus) (add-timestamps 40)))
(define chorus-e (-> chorus-base-b (variate-chorus) (add-timestamps 48)))
(define verse-b (-> verse-base-a (add-timestamps 56)))
(define verse-c (-> verse-base-b (add-timestamps 64)))
(define chorus-f (-> chorus-base-a (variate-chorus) (add-timestamps 72)))
(define chorus-g (-> chorus-base-b (variate-chorus) (add-timestamps 80)))
(define chorus-h (-> chorus-base-a (variate-chorus) (add-timestamps 88)))
(define chorus-i (-> chorus-base-a (simplify-chorus) (add-timestamps 96)))

; -------------------------------------------------------------------
; arpeggiators

; generate tons and tons of notes with arpeggiators
; (borrowed them from various spacesynth tunes I liked
; (all your arps are belong to us (sorry not sorry)))

; first, the absolutely obligatory disco bass pattern:
(define bass-arp (list
  (list 0.00 tonic 0.2 0.35)
  (list 0.25 tonic 0.25 0.15)
  (list 0.50 (+ 7 tonic) 0.2 0.25)
  (list 0.75 (+ 7 tonic) 0.25 0.05)))

(define plucks-arp (list
  (list 0.25 subtonic 0.05 0.4)
  (list 0.50 (+ 7 tonic) 0.05 0.5)
  (list 0.75 (+ 7 subdominant) 0.05 0.2)))

(define riffs-arp (list
  (list 0.75 tonic 0.25 0.2)
  (list 0.75 subdominant 0.25 0.1)
  (list 1.50 tonic 0.2 0.1)
  (list 1.50 subdominant 0.2 0.05)))

(define rhythm-arp (list
  (list 0.00 dominant 0.2 0.3)
  (list 0.25 mediant 0.1 0.2)
  (list 0.50 tonic 0.3 0.1)
  (list 0.75 dominant 0.15 0.2)
  (list 1.00 mediant 0.1 0.1)
  (list 1.25 tonic 0.25 0.2)
  (list 1.50 dominant 0.35 0.1)
  (list 1.75 subdominant 0.1 0.3)))

(define lead-arp (list
  (list 0.00 supertonic 0.45 0.25)
  (list 0.75 tonic 0.15 0.15)
  (list 1.50 supertonic 0.5 0.25)
  (list 2.25 tonic 0.2 0.2)
  (list 3.00 mediant 0.45 0.3)
  (list 3.50 supertonic 0.15 0.2)
  (list 3.75 tonic 0.2 0.1)))

(define accents-arp (list
  (list 0.25 mediant 0.05 0.1)
  (list 0.50 subdominant 0.25 0.3)
  (list 0.75 dominant 0.1 0.2)
  (list 1.00 (+ 7 tonic) 0.25 0.1)
  (list 1.25 dominant 0.05 0.2)
  (list 1.50 subdominant 0.1 0.1)
  (list 1.75 mediant 0.05 0.2)))

; but just writing arpeggiators is boring, let's shuffle them randomly:
(define (arp-note:beat arp-note) (nth 0 arp-note))
(define (arp-note:with-delta-beat arp-note delta)
  (map-indexed (λ (i x) (if (= i 0) (+ delta x) x)) arp-note))
(define (arp-note:with-delta-degree arp-note delta)
  (map-indexed (λ (i x) (if (= i 1) (+ delta x) x)) arp-note))
(define (arp-note:with-delta-length arp-note delta)
  (map-indexed (λ (i x) (if (= i 2) (+ delta x) x)) arp-note))
(define (arp-note:with-multiplied-velocity arp-note factor)
  (map-indexed (λ (i x) (if (= i 3) (* factor x) x)) arp-note))

(define (arp:with-syncopations arp (chance d10) (beat-delta 0.5))
  (map (λ (x)
    (define seems-strong-beat (= 0 (% (arp-note:beat x) 1)))
    (if (and seems-strong-beat (random chance))
      (arp-note:with-delta-beat x beat-delta)
      x))
  arp))

(define (arp:with-echo arp (interval 2) (beat-delta 0) (velocity-factor 1.0))
  (append arp
    (map
      (λ (x) (-> x
        (arp-note:with-delta-degree interval)
        (arp-note:with-delta-beat beat-delta)
        (arp-note:with-multiplied-velocity velocity-factor)))
      arp)))

(define (arp:with-length-variations arp (deltas '(-0.1 0.2)))
  (map (λ (x) (arp-note:with-delta-length x (random deltas))) arp))

; -------------------------------------------------------------------
; generating tracks

(define (make-voice-base root-key scale period progression (length bar-length))
  (reduce
    (λ (result x)
      (define beat (first x))
      (define chord (last x))
      (append result
        (map
          (λ (degree)
            (define chromatic-key (+ root-key (scale:render-key scale degree)))
            (define key (+ chromatic-key (* project:period-size period)))
            (list beat key length 0.69))
          chord)))
    ()
    progression))

(define (make-bass-voice key scale progression arp (skip-end-chance d8))
  ; for a bit more dramatic effect:
  (define (maybe-skip-last-beats chance notes)
    (if (random chance) (first (- (length notes) 8) notes) notes))
  (->> progression
    (make-voice-base key scale 2)
    (refactor:arpeggiate arp)
    (maybe-skip-last-beats skip-end-chance)))

(define (make-voice key scale progression arp (period 5)
  (drop-start-chance d99) (drop-end-chance drop-start-chance))
  (->> progression
    (make-voice-base key scale period)
    (refactor:arpeggiate arp)
    (refactor:cleanup-overlaps)
    ; a hack to treat the sequence as a number of 8-beat phrases,
    ; and drop notes with different chances at the start and
    ; at the end of each, so that repeats are more varied:
    (filter (λ (x)
      (define beat (first x))
      (define d (if (>= (% beat 8) 6) drop-end-chance drop-start-chance))
      (or (not (random d)) (= 0 (% beat 2)))))
    (refactor:join-adjacent)))

(define (track:make-chorus progression modulation voices)
  (define beat (first (first progression)))
  (define key (+ modulation chorus-key))
  (timeline:add-key beat key chorus-scale)
  (for-each
    (λ (voice)
      (define simple-triads
        (add-timestamps (repeat (length progression) triad) (/ beat 4)))
      (cond
        ((= voice 'bass)
          (track:make "wtbeep 19% square bass"
            (make-bass-voice key chorus-scale progression bass-arp d2)))
        ((= voice 'bass-muted)
          (track:make "wtbeep 6.25% square bass"
            (make-bass-voice key chorus-scale progression bass-arp d1)))
        ((= voice 'plucks)
          (define arp (-> plucks-arp
            (arp:with-echo 0 1.0)
            (arp:with-echo 0 2.0 0.75)
            (arp:with-echo -5 0.0 0.3)))
          (track:make "wtbeep 50% square plucks"
            (make-voice key chorus-scale progression arp 3 d6 d3)))
        ((= voice 'plucks-high)
          (define arp (-> plucks-arp
            (arp:with-echo 0 1.0 0.75)
            (arp:with-echo 0 2.0)
            (arp:with-echo -5 0.0 0.5)))
          (track:make "wtbeep 50% square plucks"
            (make-voice key chorus-scale progression arp 4 d6)))
        ((= voice 'rhythm)
          (define arp (-> rhythm-arp
            (arp:with-echo (random '(-4 -5 -7)) 2.0 0.5)
            (arp:with-echo (random '(-5 -7)) 4.0)
            (arp:with-syncopations d10 1.0)))
          (track:make "wtbeep 19% square rhythm"
            (make-voice key chorus-scale progression arp 3 d7 d5)))
        ((= voice 'accents)
          (define arp (-> accents-arp
            (arp:with-echo (random '(2 3)) 4.0 0.5)))
          (track:make "wtbeep 12.5% square accents"
            (make-voice key chorus-scale progression arp 2)))
        ((= voice 'lead)
          (define arp (-> lead-arp
            (arp:with-echo (random '(3 4)) 0.0 0.75)
            (arp:with-echo (random '(0 2 3 4)) (if (random d4) 4.5 4.0))
            (arp:with-echo (random '(0 -2 -3 -4)) 8.0)
            (arp:with-syncopations d30 0.75)
            (arp:with-echo (random '(0 -1)) 16.0)))
          (track:make "wtbeep 50% square lead"
            (make-voice key chorus-scale simple-triads arp 4 d16 d4)))
        ((= voice 'lead-alt)
          (define arp (-> lead-arp
            (arp:with-echo 9 0.0 0.01)
            (arp:with-echo (random '(3 -3 0)) 4.0)))
          (track:make "wtbeep 25% square lead"
            (make-voice key chorus-scale simple-triads arp 4)))
        ((= voice 'riffs)
          (define arp (-> riffs-arp
            (arp:with-echo (random '(-13 -15)) 0.0 0.1)
            (arp:with-echo (random '(-2 -3)) 2.0)
            (arp:with-echo (random '(-5 -6 -7)) 4.0 0.5)
            (arp:with-echo (random '(2 3)) 8.0 0.1)
            (arp:with-syncopations d4)))
          (track:make "wtbeep 12.5% square riffs"
            (make-voice key chorus-scale simple-triads arp 5 d12)))))
    voices))

(define (track:make-verse key-jumps modulation voices)
  (map-indexed
    (λ (index x)
      (define beat (first x))
      (define key (+ modulation (last x)))
      (define period-delta (modulo index 2))
      (timeline:add-key beat key verse-scale)
      (for-each
        (λ (voice)
          ; pick from suspended and 7/9 chords for more Mixolydian vibes
          (define chord (random (list sus4 '(1 3 7) '(1 3 9))))
          (define sequence (list (list beat chord)))
          (define bass-tonic (list (list beat triad)))
          (cond
            ((= voice 'bass)
              (track:make
                (if (or (> index 4) (= 0 (% index 2)))
                  "wtbeep 19% square bass"
                  "wtbeep 6.25% square bass")
                (make-bass-voice key verse-scale bass-tonic bass-arp d6)))
            ((= voice 'plucks)
              (track:make "wtbeep 50% square plucks"
                (make-voice key verse-scale sequence plucks-arp (- 4 period-delta) d6)))
            ((= voice 'rhythm)
              (define arp (-> rhythm-arp
                (arp:with-echo -4 0.0 0.25)
                (arp:with-echo -4 2.0 0.1)))
              (track:make "wtbeep 19% square rhythm"
                (make-voice key verse-scale sequence arp 3 d3)))
            ((= voice 'accents)
              (define arp (-> accents-arp (arp:with-echo -4 2.0 0.5)))
              (track:make "wtbeep 12.5% square accents"
                (make-voice key verse-scale sequence arp (- 3 period-delta) d12)))
            ((= voice 'lead)
              (define arp (-> lead-arp
                (arp:with-length-variations)
                (arp:with-echo -9 0.0 0.1)
                (arp:with-syncopations d7 0.75)))
              (if (or (> index 4) (= 0 (% index 2)))
                (track:make "wtbeep 25% square lead"
                  (make-voice key verse-scale sequence arp 4 d16))
                nil))
            ((= voice 'riffs)
              (define arp (-> riffs-arp
                (arp:with-echo (random '(-2 -3)) 0.0 0.5)
                (arp:with-echo (random '(-9 -11)) 2.0 0.1)))
              (track:make "wtbeep 12.5% square riffs"
                (make-voice key verse-scale sequence arp (- 5 period-delta))))))
        voices))
    key-jumps))

; update the project
(timeline:reset)

; track:remove supports wildcards and/or several names
(track:remove "wtbeep *")

(define cent (/ project:period-size 12))
(define modulation (random cent (* 3 cent)))
(track:make-chorus intro 0 (list 'bass-muted 'plucks 'rhythm 'lead-alt))
(track:make-chorus chorus-a 0 (list 'bass 'plucks 'rhythm 'lead))
(track:make-chorus chorus-b 0 (list 'bass 'plucks-high 'lead 'riffs))
(track:make-verse verse-a 0 (list 'bass 'plucks 'rhythm 'accents 'riffs))
(track:make-chorus chorus-c 0 (list 'bass 'plucks-high 'rhythm 'accents 'riffs))
(track:make-chorus bridge modulation (list 'bass-muted 'plucks 'rhythm 'lead))
(track:make-chorus chorus-d modulation (list 'bass 'rhythm 'accents 'lead))
(track:make-chorus chorus-e modulation (list 'bass 'plucks-high 'rhythm 'riffs))
(track:make-verse verse-b modulation (list 'bass 'plucks 'accents 'lead 'riffs))
(track:make-verse verse-c modulation (list 'bass 'plucks 'rhythm 'lead 'riffs))
(track:make-chorus chorus-f modulation (list 'bass 'plucks 'rhythm 'lead-alt 'riffs))
(track:make-chorus chorus-g modulation (list 'bass 'plucks-high 'rhythm 'lead))
(track:make-chorus chorus-h modulation (list 'bass 'plucks-high 'rhythm 'riffs))
(track:make-chorus chorus-i modulation (list 'bass-muted 'plucks 'rhythm))
; (no drums here, drums are drawn manually in my project)
```

## Writing parametric modifiers

*(planned, but not implemented yet; the idea is to allow you to write custom musical transformations, which will be triggered on every playback, so that you could add (slight) random variations to existing tracks)*
