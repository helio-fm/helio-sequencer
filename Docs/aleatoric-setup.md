# Aleatoric Setup

*Note: this is still a work in progress, details may change.*

The sequencer provides a small scripting DSL with Scheme-inspired syntax.

The goals are generating music from scratch relying on dice rolls and some underlying ideas, or writing custom modifiers for e.g. adding random variations to an existing piece.

The non-goals are anything related to audio processing or sound generation, or extending the UI. This tool is made with focus on algorithmic composition in mind, it derives a lot of inspiration from [Common Music](https://commonmusic.sourceforge.net), except I had hard time making sense of their code examples, so trying to implement my own take to generating music [aleatorically](https://wikipedia.org/wiki/Aleatoric_music).

### Oh no, another lisp

The syntax was more of an artistic choice for the sake of [timeless vibes](https://xkcd.com/297/), but under the hood it's a toy language that tries to be easier to read (no car/cdr/cddadr/cadddr) and encourages an old-school fp style (data is mostly immutable, functions are mostly pure, use map/filter/reduce instead of for loops), which is again for the sake of timeless vibes.

Here will be a short overview for details, but the [code examples](#example-1) are the best way to get a grasp of a language.

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

## Writing parametric modifiers

*(planned, but not implemented yet; the idea is to allow you to write custom musical transformations, which will be triggered on every playback, so that you could add (slight) random variations to existing tracks)*
