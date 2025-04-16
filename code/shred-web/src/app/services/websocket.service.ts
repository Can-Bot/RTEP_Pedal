import { Injectable, inject, PLATFORM_ID } from '@angular/core';
import { isPlatformBrowser } from '@angular/common';

@Injectable({ providedIn: 'root' })
export class WebsocketService {
  private socket!: WebSocket;
  private isBrowser = isPlatformBrowser(inject(PLATFORM_ID));

  connect(onMessage: (data: any) => void): void {
    if (!this.isBrowser) return;

    this.socket = new WebSocket('ws://shred.local:9000');

    this.socket.onopen = () => console.log('WebSocket connected');
    this.socket.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        onMessage(data);
      } catch (err) {
        console.error('Invalid JSON from server:', event.data);
      }
    };
    this.socket.onerror = (err) => console.error('WebSocket error:', err);
    this.socket.onclose = () => console.log('WebSocket closed');
  }

  sendMessage(msg: any): void {
    if (this.isBrowser && this.socket?.readyState === WebSocket.OPEN) {
      this.socket.send(JSON.stringify(msg));
    } else {
      console.warn('WebSocket is not connected');
    }
  }
}
