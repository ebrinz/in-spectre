from typing import Dict, Any, List, Optional
from dataclasses import dataclass
from datetime import datetime

@dataclass
class AS7263Data:
    """Data structure for AS7263 NIR spectral sensor readings."""
    timestamp: float
    temperature: float
    r: float  # 610nm
    s: float  # 680nm
    t: float  # 730nm
    u: float  # 760nm
    v: float  # 810nm
    w: float  # 860nm
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'AS7263Data':
        """Create an AS7263Data instance from a dictionary."""
        return cls(
            timestamp=data.get('timestamp', 0.0),
            temperature=data.get('temperature', 0.0),
            r=data.get('r', 0.0),
            s=data.get('s', 0.0),
            t=data.get('t', 0.0),
            u=data.get('u', 0.0),
            v=data.get('v', 0.0),
            w=data.get('w', 0.0)
        )
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert the AS7263Data instance to a dictionary."""
        return {
            'timestamp': self.timestamp,
            'datetime': datetime.fromtimestamp(self.timestamp).isoformat(),
            'temperature': self.temperature,
            'channels': {
                'r_610nm': self.r,
                's_680nm': self.s,
                't_730nm': self.t,
                'u_760nm': self.u,
                'v_810nm': self.v,
                'w_860nm': self.w
            }
        }

@dataclass
class AS7265XData:
    """Data structure for AS7265X 18-channel spectral sensor readings."""
    timestamp: float
    temperature: float
    # AS7265X channels (18 channels across UV, Visible, NIR)
    a: float  # 410nm
    b: float  # 435nm
    c: float  # 460nm
    d: float  # 485nm
    e: float  # 510nm
    f: float  # 535nm
    g: float  # 560nm
    h: float  # 585nm
    i: float  # 610nm
    j: float  # 645nm
    k: float  # 680nm
    l: float  # 705nm
    m: float  # 730nm
    n: float  # 760nm
    o: float  # 810nm
    p: float  # 860nm
    q: float  # 900nm
    r: float  # 940nm
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'AS7265XData':
        """Create an AS7265XData instance from a dictionary."""
        return cls(
            timestamp=data.get('timestamp', 0.0),
            temperature=data.get('temperature', 0.0),
            a=data.get('a', 0.0),
            b=data.get('b', 0.0),
            c=data.get('c', 0.0),
            d=data.get('d', 0.0),
            e=data.get('e', 0.0),
            f=data.get('f', 0.0),
            g=data.get('g', 0.0),
            h=data.get('h', 0.0),
            i=data.get('i', 0.0),
            j=data.get('j', 0.0),
            k=data.get('k', 0.0),
            l=data.get('l', 0.0),
            m=data.get('m', 0.0),
            n=data.get('n', 0.0),
            o=data.get('o', 0.0),
            p=data.get('p', 0.0),
            q=data.get('q', 0.0),
            r=data.get('r', 0.0)
        )
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert the AS7265XData instance to a dictionary."""
        return {
            'timestamp': self.timestamp,
            'datetime': datetime.fromtimestamp(self.timestamp).isoformat(),
            'temperature': self.temperature,
            'channels': {
                'a_410nm': self.a,
                'b_435nm': self.b,
                'c_460nm': self.c,
                'd_485nm': self.d,
                'e_510nm': self.e,
                'f_535nm': self.f,
                'g_560nm': self.g,
                'h_585nm': self.h,
                'i_610nm': self.i,
                'j_645nm': self.j,
                'k_680nm': self.k,
                'l_705nm': self.l,
                'm_730nm': self.m,
                'n_760nm': self.n,
                'o_810nm': self.o,
                'p_860nm': self.p,
                'q_900nm': self.q,
                'r_940nm': self.r
            }
        }