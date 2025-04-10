import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { WebsocketService } from './services/websocket.service';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'shred-web';
  notes = ['C', 'D', 'E', 'F', 'G', 'A', 'B'];
  lastNote = '';

  constructor(private ws: WebsocketService) {
    this.ws.connect((msg: any) => {
      if (msg.status === 'note_received') {
        this.lastNote = msg.note;
      }
    });
  }

  playNote(note: string): void {
    this.ws.sendMessage({
      action: 'play_note',
      note,
      velocity: 1.0
    });
  }
}
