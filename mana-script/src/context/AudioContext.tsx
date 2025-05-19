import React, { createContext, useContext, useState, useEffect, ReactNode } from 'react';
import { Howl } from 'howler';
import useStore from '../store';

interface AudioContextType {
  playAmbientSound: () => void;
  stopAmbientSound: () => void;
  playDaySound: () => void;
  playNightSound: () => void;
}

const AudioContext = createContext<AudioContextType | null>(null);

const useAudio = () => {
  const context = useContext(AudioContext);
  if (!context) {
    throw new Error('useAudio must be used within an AudioProvider');
  }
  return context;
};

interface AudioProviderProps {
  children: ReactNode;
}

const AudioProvider = ({ children }: AudioProviderProps) => {
  const { isMuted, isDaytime } = useStore();
  const [daySound, setDaySound] = useState<Howl | null>(null);
  const [nightSound, setNightSound] = useState<Howl | null>(null);
  const [currentSound, setCurrentSound] = useState<'day' | 'night' | null>(null);

  useEffect(() => {
    const daySoundHowl = new Howl({
      src: ['https://cdn.freesound.org/previews/537/537061_4921277-lq.mp3'],
      loop: true,
      volume: 0.4,
    });

    const nightSoundHowl = new Howl({
      src: ['https://cdn.freesound.org/previews/517/517742_2563270-lq.mp3'],
      loop: true,
      volume: 0.4,
    });

    setDaySound(daySoundHowl);
    setNightSound(nightSoundHowl);

    return () => {
      daySoundHowl.unload();
      nightSoundHowl.unload();
    };
  }, []);

  useEffect(() => {
    if (isMuted) {
      stopAmbientSound();
    } else if (currentSound) {
      if (currentSound === 'day') {
        playDaySound();
      } else {
        playNightSound();
      }
    }
  }, [isMuted]);

  useEffect(() => {
    if (!isMuted) {
      if (isDaytime) {
        playDaySound();
      } else {
        playNightSound();
      }
    }
  }, [isDaytime]);

  const playDaySound = () => {
    if (nightSound) nightSound.stop();
    if (daySound && !isMuted) {
      daySound.play();
      setCurrentSound('day');
    }
  };

  const playNightSound = () => {
    if (daySound) daySound.stop();
    if (nightSound && !isMuted) {
      nightSound.play();
      setCurrentSound('night');
    }
  };

  const playAmbientSound = () => {
    if (isDaytime) {
      playDaySound();
    } else {
      playNightSound();
    }
  };

  const stopAmbientSound = () => {
    if (daySound) daySound.stop();
    if (nightSound) nightSound.stop();
  };

  return (
    <AudioContext.Provider
      value={{
        playAmbientSound,
        stopAmbientSound,
        playDaySound,
        playNightSound,
      }}
    >
      {children}
    </AudioContext.Provider>
  );
};

export { AudioProvider, useAudio };